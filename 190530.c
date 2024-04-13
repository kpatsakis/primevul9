
#ifdef CONFIG_BLK_DEV_ZONED
static inline bool f2fs_blkz_is_seq(struct f2fs_sb_info *sbi, int devi,
				    block_t blkaddr)
{
	unsigned int zno = blkaddr >> sbi->log_blocks_per_blkz;
