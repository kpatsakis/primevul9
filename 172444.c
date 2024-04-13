btrfs_get_bdev_and_sb(const char *device_path, fmode_t flags, void *holder,
		      int flush, struct block_device **bdev,
		      struct buffer_head **bh)
{
	int ret;

	*bdev = blkdev_get_by_path(device_path, flags, holder);

	if (IS_ERR(*bdev)) {
		ret = PTR_ERR(*bdev);
		goto error;
	}

	if (flush)
		filemap_write_and_wait((*bdev)->bd_inode->i_mapping);
	ret = set_blocksize(*bdev, BTRFS_BDEV_BLOCKSIZE);
	if (ret) {
		blkdev_put(*bdev, flags);
		goto error;
	}
	invalidate_bdev(*bdev);
	*bh = btrfs_read_dev_super(*bdev);
	if (IS_ERR(*bh)) {
		ret = PTR_ERR(*bh);
		blkdev_put(*bdev, flags);
		goto error;
	}

	return 0;

error:
	*bdev = NULL;
	*bh = NULL;
	return ret;
}