static inline bool f2fs_skip_inode_update(struct inode *inode, int dsync)
{
	bool ret;

	if (dsync) {
		struct f2fs_sb_info *sbi = F2FS_I_SB(inode);

		spin_lock(&sbi->inode_lock[DIRTY_META]);
		ret = list_empty(&F2FS_I(inode)->gdirty_list);
		spin_unlock(&sbi->inode_lock[DIRTY_META]);
		return ret;
	}
	if (!is_inode_flag_set(inode, FI_AUTO_RECOVER) ||
			file_keep_isize(inode) ||
			i_size_read(inode) & ~PAGE_MASK)
		return false;

	if (!timespec64_equal(F2FS_I(inode)->i_disk_time, &inode->i_atime))
		return false;
	if (!timespec64_equal(F2FS_I(inode)->i_disk_time + 1, &inode->i_ctime))
		return false;
	if (!timespec64_equal(F2FS_I(inode)->i_disk_time + 2, &inode->i_mtime))
		return false;
	if (!timespec64_equal(F2FS_I(inode)->i_disk_time + 3,
						&F2FS_I(inode)->i_crtime))
		return false;

	down_read(&F2FS_I(inode)->i_sem);
	ret = F2FS_I(inode)->last_disk_size == i_size_read(inode);
	up_read(&F2FS_I(inode)->i_sem);

	return ret;
}