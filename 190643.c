static inline void dec_valid_inode_count(struct f2fs_sb_info *sbi)
{
	percpu_counter_dec(&sbi->total_valid_inode_count);
}