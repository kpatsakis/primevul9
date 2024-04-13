static void default_options(struct f2fs_sb_info *sbi)
{
	/* init some FS parameters */
	sbi->active_logs = NR_CURSEG_TYPE;

	set_opt(sbi, BG_GC);
	set_opt(sbi, INLINE_DATA);
	set_opt(sbi, EXTENT_CACHE);

#ifdef CONFIG_F2FS_FS_XATTR
	set_opt(sbi, XATTR_USER);
#endif
#ifdef CONFIG_F2FS_FS_POSIX_ACL
	set_opt(sbi, POSIX_ACL);
#endif
}