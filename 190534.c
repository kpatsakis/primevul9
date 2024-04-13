static inline bool is_idle(struct f2fs_sb_info *sbi, int type)
{
	if (sbi->gc_mode == GC_URGENT)
		return true;

	if (get_pages(sbi, F2FS_RD_DATA) || get_pages(sbi, F2FS_RD_NODE) ||
		get_pages(sbi, F2FS_RD_META) || get_pages(sbi, F2FS_WB_DATA) ||
		get_pages(sbi, F2FS_WB_CP_DATA) ||
		get_pages(sbi, F2FS_DIO_READ) ||
		get_pages(sbi, F2FS_DIO_WRITE))
		return false;

	if (type != DISCARD_TIME && SM_I(sbi) && SM_I(sbi)->dcc_info &&
			atomic_read(&SM_I(sbi)->dcc_info->queued_discard))
		return false;

	if (SM_I(sbi) && SM_I(sbi)->fcc_info &&
			atomic_read(&SM_I(sbi)->fcc_info->queued_flush))
		return false;

	return f2fs_time_over(sbi, type);
}