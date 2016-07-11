
/* Compare bio1 and bio2, print the differences between them, if exists. */
void compare_bio(struct bio *bio1,struct bio *bio2)
{
	if (bio1->bi_sector != bio2->bi_sector) {
		printk(KERN_NOTICE "bi_sector: %llu != %llu\n", 
				bio1->bi_sector, bio2->bi_sector);
	}
	if (bio1->bi_next != bio2->bi_next) {
		printk(KERN_NOTICE "bi_next: %p != %p\n",
				bio1->bi_next, bio2->bi_next);
	}
	if (bio1->bi_bdev != bio2->bi_bdev) {
		printk(KERN_NOTICE "bi_bdev: %p != %p\n",
				bio1->bi_bdev, bio2->bi_bdev);
	}
	if (bio1->bi_flags != bio2->bi_flags) {
		printk(KERN_NOTICE "bi_flags: %lu != %lu\n",
				bio1->bi_flags, bio2->bi_flags);
	}
	if (bio1->bi_rw != bio2->bi_rw) {
		printk(KERN_NOTICE "bi_rw: %lu != %lu\n",
				bio1->bi_rw, bio2->bi_rw);
	}
	if (bio1->bi_vcnt != bio2->bi_vcnt) {
		printk(KERN_NOTICE "bi_vcnt: %u != %u\n",
				bio1->bi_vcnt, bio2->bi_vcnt);
	}
	if (bio1->bi_idx != bio2->bi_idx) {
		printk(KERN_NOTICE "bi_idx: %u != %u\n",
				bio1->bi_idx, bio2->bi_idx);
	}
	if (bio1->bi_phys_segments != bio2->bi_phys_segments) {
		printk(KERN_NOTICE "bi_phys_segments: %u != %u\n", 
				bio1->bi_phys_segments, bio2->bi_phys_segments);
	}
	if (bio1->bi_hw_segments != bio2->bi_hw_segments) {
		printk(KERN_NOTICE "bi_hw_segments: %u != %u\n", 
				bio1->bi_hw_segments, bio2->bi_hw_segments);
	}
	if (bio1->bi_size != bio2->bi_size) {
		printk(KERN_NOTICE "bi_size: %u != %u\n", 
				bio1->bi_size, bio2->bi_size);
	}
	if (bio1->bi_hw_front_size != bio2->bi_hw_front_size) {
		printk(KERN_NOTICE "bi_hw_front_size: %u != %u\n", 
				bio1->bi_hw_front_size, bio2->bi_hw_front_size);
	}
	if (bio1->bi_hw_back_size != bio2->bi_hw_back_size) {
		printk(KERN_NOTICE "bi_hw_back_size: %u != %u\n", 
				bio1->bi_hw_back_size, bio2->bi_hw_back_size);
	}
	if (bio1->bi_max_vecs != bio2->bi_max_vecs) {
		printk(KERN_NOTICE "bi_max_vecs: %u != %u\n",
				bio1->bi_max_vecs, bio2->bi_max_vecs);
	}
	if (bio1->bi_io_vec != bio2->bi_io_vec) {
		printk(KERN_NOTICE "bi_io_vec: %p != %p\n", 
				bio1->bi_io_vec, bio2->bi_io_vec);
	}
	if (bio1->bi_end_io != bio2->bi_end_io) {
		printk(KERN_NOTICE "bi_end_io: %p != %p\n", 
				bio1->bi_end_io, bio2->bi_end_io);
	}
	if (atomic_read(&bio1->bi_cnt) != atomic_read(&bio2->bi_cnt)) {
		printk(KERN_NOTICE "bi_cnt: %d != %d\n", 
				atomic_read(&bio1->bi_cnt), atomic_read(&bio2->bi_cnt));
	}
	if (bio1->bi_private != bio2->bi_private) {
		printk(KERN_NOTICE "bi_private: %p != %p\n", 
				bio1->bi_private, bio2->bi_private);
	}
	if (bio1->bi_destructor != bio2->bi_destructor) {
		printk(KERN_NOTICE "bi_destructor: %p != %p\n", 
				bio1->bi_destructor, bio2->bi_destructor);
	}
}
