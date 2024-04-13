static inline void inc_valid_inode_count(struct f2fs_sb_info *sbi)
{
	percpu_counter_inc(&sbi->total_valid_inode_count);
}