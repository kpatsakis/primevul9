#endif

static inline bool is_journalled_quota(struct f2fs_sb_info *sbi)
{
#ifdef CONFIG_QUOTA
	if (f2fs_sb_has_quota_ino(sbi))
		return true;
	if (F2FS_OPTION(sbi).s_qf_names[USRQUOTA] ||
		F2FS_OPTION(sbi).s_qf_names[GRPQUOTA] ||
		F2FS_OPTION(sbi).s_qf_names[PRJQUOTA])
		return true;
#endif