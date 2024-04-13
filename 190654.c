static inline bool f2fs_is_multi_device(struct f2fs_sb_info *sbi)
{
	return sbi->s_ndevs > 1;
}