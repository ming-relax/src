/*
 * 1blkdrive/sbs.h -- structures for the sector back shift module
 *
 * Copyright(c) 2011 NRCHPC, Inc. All rights reserved. 
 *
 * Author: Zhang Bingyin <zhangbingyin@nrchpc.ac.cn>
 *
 */
#include <linux/bio.h>

struct sbs_dev{
	struct request_queue *queue;	/* The device request queue */
	struct gendisk *gd;				/* The gendisk structure */
};

/*
 * bio context
 */
struct sbs_bio_context{
	struct bio *parent;			/* save the old bio */
	atomic_t ref_cnt;				/* reference count */
	unsigned int bytes_done;	/* save bytes_done for the first returned bio */
	int error;						/* save error for the first returned bio */
	spinlock_t lock;				/* protect this structrue */
};
