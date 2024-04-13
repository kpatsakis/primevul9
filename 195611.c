static int block_device_ejected(struct super_block *sb)
{
	struct inode *bd_inode = sb->s_bdev->bd_inode;
	struct backing_dev_info *bdi = bd_inode->i_mapping->backing_dev_info;

	return bdi->dev == NULL;
}