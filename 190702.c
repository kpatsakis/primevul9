static inline void inc_page_count(struct f2fs_sb_info *sbi, int count_type)
{
	atomic_inc(&sbi->nr_pages[count_type]);

	if (count_type == F2FS_DIRTY_DENTS ||
			count_type == F2FS_DIRTY_NODES ||
			count_type == F2FS_DIRTY_META ||
			count_type == F2FS_DIRTY_QDATA ||
			count_type == F2FS_DIRTY_IMETA)
		set_sbi_flag(sbi, SBI_IS_DIRTY);
}