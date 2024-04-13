static int f2fs_swap_activate(struct swap_info_struct *sis, struct file *file,
				sector_t *span)
{
	struct inode *inode = file_inode(file);
	int ret;

	if (!S_ISREG(inode->i_mode))
		return -EINVAL;

	if (f2fs_readonly(F2FS_I_SB(inode)->sb))
		return -EROFS;

	ret = f2fs_convert_inline_inode(inode);
	if (ret)
		return ret;

	ret = check_swap_activate(file, sis->max);
	if (ret)
		return ret;

	set_inode_flag(inode, FI_PIN_FILE);
	f2fs_precache_extents(inode);
	f2fs_update_time(F2FS_I_SB(inode), REQ_TIME);
	return 0;
}