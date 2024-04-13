static inline s64 valid_inode_count(struct f2fs_sb_info *sbi)
{
	return percpu_counter_sum_positive(&sbi->total_valid_inode_count);
}