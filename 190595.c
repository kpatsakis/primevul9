}

static inline bool f2fs_bdev_support_discard(struct block_device *bdev)
{
	return blk_queue_discard(bdev_get_queue(bdev)) ||