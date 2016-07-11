/*
 * New Member training,May 2011. Title:Sector back shift module.
 *
 * CopyRight (c) 2011 NRCHPC, Inc. All rights reserved.
 *
 * Author: Zhang Bingyin <zhangbingyin@nrchpc.ac.cn>
 *
 */

#include "klog.h"
#include "sbs.h"
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/blkdev.h>
#include <linux/genhd.h>
#include <linux/kthread.h>


		//set_current_state(TASK_INTERRUPTIBLE);
#ifndef SLEEP_MILLI_SEC
#define SLEEP_MILLI_SEC(nMilliSec)                  \
	do{                                                     \
		long timeout = (nMilliSec) * HZ / 1000; \
		while (timeout > 0) {                           \
			timeout = schedule_timeout(timeout);    \
		}                                                       \
	} while (0);
#endif

static void print_io_counter( void  );
#define MY_THREAD_CNT 5
static struct task_struct *MyThread[MY_THREAD_CNT];

static int print_count(void *data)
{
	while (!kthread_should_stop()) {
		SLEEP_MILLI_SEC(800);
		print_io_counter();
	}   
	return 0;
}

static int init_kthread(void)
{
	int i;
	for(i =0; i < MY_THREAD_CNT; i++)
		MyThread[i] = kthread_run(print_count, NULL, "mythread");
	return 0;
}

static void exit_kthread(void)
{
	int i;
	for(i =0; i < MY_THREAD_CNT; i++){
		if (MyThread[i]) {
			printk("stop MyThread\n");
			kthread_stop(MyThread[i]);
		}   
	}
}

/* Major Number */
static int sbs_major = 0;
/* Point to my device structure */
static struct sbs_dev *Device = NULL;
/* My device sector size(bytes) */
static int hardsect_size = 0;
/* Device size in sectors */
static int nsectors = 0;
#define ORIG_DEV "/dev/vg0/lv_2g"
static struct block_device *disk_bdev = NULL;
/* Sector back shift number */
static int shift_sector_num = 1;

struct io_counter_s{
	atomic_t cnt;
	spinlock_t lock;
};

struct io_counter_s io_counter;

/* My device only has 1 partition */
#define SBS_MINORS 1

static void dec_io_counter( void )
{
	spin_lock(&io_counter.lock);
	printk(KERN_NOTICE "%s, pid:%d\n", __func__, current->pid);
	atomic_dec(&io_counter.cnt);
	spin_unlock(&io_counter.lock);
}

static void inc_io_counter( void  )
{
//don't lock here, otherwise the the io can't be submitted in time and it will be hard for a callback to interrupt a normal process.
//	spin_lock(&io_counter.lock);
	atomic_inc(&io_counter.cnt);
//	spin_unlock(&io_counter.lock);
}

static void print_io_counter( void  )
{
	int i;
	char buf[1024];
	set_current_state(TASK_INTERRUPTIBLE);
	spin_lock(&io_counter.lock);
	for(i=0; i < 100; i ++){
		printk(KERN_NOTICE "id: %d, io count: %d.\n", i, atomic_read(&io_counter.cnt));
		memset(buf, i, 1024);
	}
	spin_unlock(&io_counter.lock);
}

static void init_io_counter( void )
{
	spin_lock_init(&io_counter.lock);
	atomic_set(&io_counter.cnt, 0);
}

void sbs_context_get(struct sbs_bio_context *bio_context)
{
	atomic_inc(&bio_context->ref_cnt);
}

void sbs_context_put(struct sbs_bio_context *bio_context)
{
	if (atomic_dec_and_test(&bio_context->ref_cnt)) {
		if (bio_context->parent->bi_end_io) {
			bio_endio(bio_context->parent, bio_context->bytes_done,
					(bio_context->error == 0?0:-EIO));
		}
		kfree(bio_context);
	}
}


/*
 * sbs_bio_callback - callback function when bio returns. 
 */
static int sbs_bio_callback(struct bio *bio, unsigned int bytes_done, int error)
{
	struct sbs_bio_context *tmp_bio_context = bio->bi_private;
	/* just for test */
	klog(WARN, "step in function sbs_bio_callback.\n");
	spin_lock(&tmp_bio_context->lock);
	tmp_bio_context->error |= error;
	tmp_bio_context->bytes_done += bytes_done;
	spin_unlock(&tmp_bio_context->lock);
	dec_io_counter( );
	sbs_context_put(tmp_bio_context);
	bio_put(bio);
	printk(KERN_NOTICE "%s, pid: %d\n", __func__, current->pid);
	return 0;
}

/*
 * save_bio_context - save my bio context
 */
static int save_bio_context(struct bio *bio, struct bio *bio1, struct bio *bio2)
{
	struct sbs_bio_context *tmp_bio_context = NULL;
	tmp_bio_context = kmalloc(sizeof(struct sbs_bio_context), GFP_KERNEL);
	if (NULL == tmp_bio_context) {
		printk(KERN_NOTICE "kmalloc in save_bio_context failure.\n");
		return -ENOMEM;
	}
	spin_lock_init(&(tmp_bio_context->lock));
	tmp_bio_context->parent = bio;
	atomic_set(&tmp_bio_context->ref_cnt, 0);
	tmp_bio_context->error = 0;
	tmp_bio_context->bytes_done = 0;

	sbs_context_get(tmp_bio_context);
	bio1->bi_private = tmp_bio_context;
	bio1->bi_end_io = sbs_bio_callback;

	sbs_context_get(tmp_bio_context);
	bio2->bi_private = tmp_bio_context;
	bio2->bi_end_io = sbs_bio_callback;
	/* just for test */
	klog(WARN, "In function save_bio_context\n");
	return 0;
}

/*
 * sbs_split_bio - Create bio1 and bio2, then modify field bi_io_vec in bio1 and bio2,
 * according to the same field in bio.
 */
static void sbs_split_bio(struct bio *bio, struct bio ** bio1, struct bio ** bio2)
{
//	int first_half_bytes = (nsectors - bio->bi_sector - shift_sector_num) * hardsect_size;
	int first_half_bytes = hardsect_size;
	/* used to save the split position */
	int split_page = 0, split_offset = 0;
	unsigned int sum = 0;
	int i = 0, bio1_vcnt = 0, bio2_vcnt = 0;
	struct bio_vec *bvec = NULL;

	/* search the split position */
	bio_for_each_segment(bvec, bio, i) {
		sum += bvec->bv_len;
		if (sum >= first_half_bytes) {
			if (sum > first_half_bytes) {/* Need to split in page */
				/* just for test */
				klog(WARN, "Need to split in page(in sbs_split_bio)\n");
				split_page = i;
				/* split_offset=bvec->bv_offset+(first_half_bytes-(sum-bvec->bv_len) */
				split_offset = bvec->bv_offset + first_half_bytes + bvec->bv_len - sum;
			} else {
				/* just for test */
				klog(WARN, "don't need to split in page(in sbs_split_bio)\n");
				split_page = i + 1;
				split_offset = 0;
			}
			break;
		}
	}

	bio1_vcnt = split_page;
	if (0 != split_offset) {
		bio1_vcnt += 1;
	}
	bio2_vcnt = bio->bi_vcnt - split_page;

	*bio1 = bio_alloc(GFP_NOIO, bio1_vcnt);
	(*bio1)->bi_sector = bio->bi_sector + shift_sector_num;
	(*bio1)->bi_bdev = disk_bdev; 
	(*bio1)->bi_vcnt = bio1_vcnt;
	(*bio1)->bi_idx = 0;
	(*bio1)->bi_size = first_half_bytes;

	(*bio2) = bio_alloc(GFP_NOIO, bio2_vcnt);
	(*bio2)->bi_sector = 0;
	(*bio2)->bi_bdev = disk_bdev; 
	(*bio2)->bi_vcnt = bio2_vcnt;
	(*bio1)->bi_idx = 0;
	(*bio2)->bi_size = bio->bi_size - first_half_bytes;

	bio_for_each_segment(bvec, bio, i) {
		if (i < split_page) {
			/* just for test */
			klog(WARN, "Modifying bio1(in sbs_split_bio)\n");
			(*bio1)->bi_io_vec[i].bv_page = bvec->bv_page;
			(*bio1)->bi_io_vec[i].bv_len = bvec->bv_len;
			(*bio1)->bi_io_vec[i].bv_offset = bvec->bv_offset;
		} else if (i > split_page) {
			/* just for test */
			klog(WARN, "Modifying bio2(in sbs_split_bio)\n");
			(*bio2)->bi_io_vec[i - split_page].bv_page = bvec->bv_page;
			(*bio2)->bi_io_vec[i - split_page].bv_len = bvec->bv_len;
			(*bio2)->bi_io_vec[i - split_page].bv_offset = bvec->bv_offset;
		} else {
			/* just for test */
			klog(WARN, "Modifying bio1 and bio2(in sbs_split_bio)\n");
			if (0 == split_offset) {
				(*bio2)->bi_io_vec[0].bv_page = bvec->bv_page;
				(*bio2)->bi_io_vec[0].bv_len = bvec->bv_len;
				(*bio2)->bi_io_vec[0].bv_offset = bvec->bv_offset;
			} else {
				(*bio1)->bi_io_vec[i].bv_page = bvec->bv_page;
				(*bio1)->bi_io_vec[i].bv_len = split_offset - bvec->bv_offset;
				(*bio1)->bi_io_vec[i].bv_offset = bvec->bv_offset;

				(*bio2)->bi_io_vec[0].bv_page = bvec->bv_page;
				(*bio2)->bi_io_vec[0].bv_len = bvec->bv_len - (*bio1)->bi_io_vec[i].bv_len;
				(*bio2)->bi_io_vec[0].bv_offset = split_offset;
			}
		}
	}
}

typedef struct single_ctx_s{
	void * private;
	int (*callback) (struct bio *bio, unsigned int bytes_done, int error);
}single_ctx_t;

int single_bio_callback (struct bio *bio, unsigned int bytes_done, int error)
{
	 single_ctx_t * ctx = bio->bi_private;
	 dec_io_counter();
	 bio->bi_private = ctx->private;
	 bio->bi_end_io = ctx->callback;
	 bio_endio(bio, bytes_done, error);
	 kfree(ctx);
	 return error;
}


void change_for_single_bio(struct bio* bio)
{
	single_ctx_t * ctx = kmalloc(sizeof(single_ctx_t), GFP_KERNEL);	

	ctx->private = bio->bi_private;
	ctx->callback = bio->bi_end_io;
	bio->bi_private = ctx;
	bio->bi_end_io = single_bio_callback;

}
/* 
 * sbs_make_request - my make_request function
 */
static int sbs_make_request(request_queue_t *q, struct bio *bio)
{
	/* just for test(3;) */
	/*int i = 0;
	klog(WARN, "bi_sector=%d,bi_size=%d\n", 
			(int)(bio->bi_sector), bio->bi_size);
	for(i=0;i<bio->bi_vcnt;i++) {
		klog(WARN, "\tbi_io_vec[%d].bv_len=%d\n", 
				i, (bio->bi_io_vec[i]).bv_len);
	}*/
	printk(KERN_NOTICE "%s, pid: %d\n", __func__, current->pid);
	inc_io_counter();
	if ((bio->bi_sector + bio->bi_size / hardsect_size > nsectors)
			|| bio->bi_sector > nsectors - 1) {/* the index of any sector 
															  must locate in [0,nsectors-1]*/
		printk(KERN_NOTICE "index out of [0,nsectors-1](in sbs_make_request)\n");
		bio_endio(bio, bio->bi_size, -EIO);
		return 0;
	}
	if ((bio->bi_sector + shift_sector_num < nsectors) 
			&& ((bio->bi_sector + bio->bi_size / hardsect_size + shift_sector_num)
				> nsectors)) {/*Need to split bio*/
		struct bio *bio1 = NULL, *bio2 = NULL;
		int retn;
		/* just for test */
		printk(KERN_NOTICE  "split bio(in sbs_make_request)\n");
		sbs_split_bio(bio, &bio1, &bio2);
		retn = save_bio_context(bio, bio1, bio2);
		if (-ENOMEM == retn) {
			bio_put(bio1);
			bio_put(bio2);
			bio_endio(bio, bio->bi_size, -ENOMEM);
			return 0;
		}
		submit_bio(bio->bi_rw, bio1);
		submit_bio(bio->bi_rw, bio2);
		/*generic_make_request(bio1);
		generic_make_request(bio2);*/
		return 0;
	}
	/* just for test */
	printk(KERN_NOTICE  "does not need to split bio(in sbs_make_request)\n");
	bio->bi_bdev = disk_bdev;
	bio->bi_sector = (bio->bi_sector + shift_sector_num) % nsectors;
	change_for_single_bio(bio);
	return 1;
}

static int sbs_open(struct inode *inode, struct file *filp)
{
	/* just for test */
	klog(WARN, "in function sbs_open\n");
	filp->private_data = inode->i_bdev->bd_disk->private_data;
	return 0;
}

static int sbs_release(struct inode *inode, struct file *filp)
{
	/* just for test */
	klog(WARN, "in function sbs_release\n");
	return 0;
}

static struct block_device_operations sbs_ops = {
	.owner	= THIS_MODULE,
	.open		= sbs_open,
	.release	= sbs_release
};

static void setup_device(void)
{
	memset(Device, 0, sizeof(struct sbs_dev));
	Device->queue = blk_alloc_queue(GFP_KERNEL);
	if (NULL == Device->queue) {
		goto out_vfree;
	}
	blk_queue_make_request(Device->queue, sbs_make_request);
	blk_queue_hardsect_size(Device->queue, hardsect_size);
	Device->queue->queuedata = Device;
	/* Add the gendisk structure */
	Device->gd = alloc_disk(SBS_MINORS);
	if (!Device->gd) {
		printk(KERN_NOTICE "alloc_disk in setup_device failure.\n");
		goto out_vfree;
	}
	Device->gd->major = sbs_major;
	Device->gd->first_minor = 0;
	Device->gd->fops = &sbs_ops;
	Device->gd->queue = Device->queue;
	Device->gd->private_data = Device;
	snprintf(Device->gd->disk_name, 32, "sbs");
	set_capacity(Device->gd, nsectors);
	add_disk(Device->gd);
out_vfree:
	;
}


static int __init sbs_init(void)
{
	/*just for test*/
	klog(WARN, "Hello,I'm sbs_init,I'm starting.\n");
	sbs_major = register_blkdev(sbs_major, "sbs");
	/*just for test*/
	klog(WARN, "sbs_major = %d\n", sbs_major);
	if (sbs_major <= 0) {
		printk(KERN_WARNING "sbs: unable to get major number.\n");
		return -EBUSY;
	}
	Device = kmalloc(sizeof(struct sbs_dev), GFP_KERNEL);
	if (NULL == Device) {
		unregister_blkdev(sbs_major, "sbs");
		return -ENOMEM;
	}
	disk_bdev = open_bdev_excl(ORIG_DEV, FMODE_WRITE | FMODE_READ, NULL);
	if (IS_ERR(disk_bdev)) {
		printk(KERN_ALERT "open %s failed.\n", ORIG_DEV);
		return ENOENT;
	}
	init_io_counter();
	init_kthread();
	nsectors = get_capacity(disk_bdev->bd_disk);
	hardsect_size = queue_hardsect_size(disk_bdev->bd_disk->queue);
	setup_device();
	return 0;
}

static void __exit sbs_exit(void)
{
	/*just for test*/
	klog(WARN, "Hello,I'm sbs_exit,I'm going.Bye!\n");
	if (disk_bdev) {
		close_bdev_excl(disk_bdev);
	}
	if (Device->gd) {
		del_gendisk(Device->gd);
		put_disk(Device->gd);
	}
	if (Device->queue) {
		blk_put_queue(Device->queue);
	}
	unregister_blkdev(sbs_major, "sbs");
	if (NULL != Device) {
		kfree(Device);
	}
	exit_kthread();
}

module_init(sbs_init);
module_exit(sbs_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Bingyin Zhang");
