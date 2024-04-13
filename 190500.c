static inline s64 get_pages(struct f2fs_sb_info *sbi, int count_type)
{
	return atomic_read(&sbi->nr_pages[count_type]);
}