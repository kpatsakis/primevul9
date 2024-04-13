xfs_blkdev_put(
	struct block_device	*bdev)
{
	if (bdev)
		blkdev_put(bdev, FMODE_READ|FMODE_WRITE|FMODE_EXCL);
}