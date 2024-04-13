static inline bool f2fs_may_extent_tree(struct inode *inode)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);

	if (!test_opt(sbi, EXTENT_CACHE) ||
			is_inode_flag_set(inode, FI_NO_EXTENT))
		return false;

	/*
	 * for recovered files during mount do not create extents
	 * if shrinker is not registered.
	 */
	if (list_empty(&sbi->s_list))
		return false;

	return S_ISREG(inode->i_mode);
}