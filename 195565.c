static int ext4_blkdev_put(struct block_device *bdev)
{
	return blkdev_put(bdev, FMODE_READ|FMODE_WRITE|FMODE_EXCL);
}