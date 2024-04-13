static inline bool __allow_reserved_blocks(struct f2fs_sb_info *sbi,
					struct inode *inode, bool cap)
{
	if (!inode)
		return true;
	if (!test_opt(sbi, RESERVE_ROOT))
		return false;
	if (IS_NOQUOTA(inode))
		return true;
	if (uid_eq(F2FS_OPTION(sbi).s_resuid, current_fsuid()))
		return true;
	if (!gid_eq(F2FS_OPTION(sbi).s_resgid, GLOBAL_ROOT_GID) &&
					in_group_p(F2FS_OPTION(sbi).s_resgid))
		return true;
	if (cap && capable(CAP_SYS_RESOURCE))
		return true;
	return false;
}