static inline void dec_valid_block_count(struct f2fs_sb_info *sbi,
						struct inode *inode,
						block_t count)
{
	blkcnt_t sectors = count << F2FS_LOG_SECTORS_PER_BLOCK;

	spin_lock(&sbi->stat_lock);
	f2fs_bug_on(sbi, sbi->total_valid_block_count < (block_t) count);
	sbi->total_valid_block_count -= (block_t)count;
	if (sbi->reserved_blocks &&
		sbi->current_reserved_blocks < sbi->reserved_blocks)
		sbi->current_reserved_blocks = min(sbi->reserved_blocks,
					sbi->current_reserved_blocks + count);
	spin_unlock(&sbi->stat_lock);
	if (unlikely(inode->i_blocks < sectors)) {
		f2fs_warn(sbi, "Inconsistent i_blocks, ino:%lu, iblocks:%llu, sectors:%llu",
			  inode->i_ino,
			  (unsigned long long)inode->i_blocks,
			  (unsigned long long)sectors);
		set_sbi_flag(sbi, SBI_NEED_FSCK);
		return;
	}
	f2fs_i_blocks_write(inode, count, false, true);
}