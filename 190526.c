static inline void dec_page_count(struct f2fs_sb_info *sbi, int count_type)
{
	atomic_dec(&sbi->nr_pages[count_type]);
}