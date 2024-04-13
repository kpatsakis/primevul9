static inline block_t valid_user_blocks(struct f2fs_sb_info *sbi)
{
	return sbi->total_valid_block_count;
}