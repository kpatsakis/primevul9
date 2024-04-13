bool f2fs_is_valid_blkaddr(struct f2fs_sb_info *sbi,
					block_t blkaddr, int type);
static inline void verify_blkaddr(struct f2fs_sb_info *sbi,
					block_t blkaddr, int type)
{
	if (!f2fs_is_valid_blkaddr(sbi, blkaddr, type)) {
		f2fs_err(sbi, "invalid blkaddr: %u, type: %d, run fsck to fix.",
			 blkaddr, type);
		f2fs_bug_on(sbi, 1);