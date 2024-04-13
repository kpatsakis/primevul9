static inline bool f2fs_crc_valid(struct f2fs_sb_info *sbi, __u32 blk_crc,
				  void *buf, size_t buf_size)
{
	return f2fs_crc32(sbi, buf, buf_size) == blk_crc;
}