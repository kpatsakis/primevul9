static inline u32 f2fs_chksum(struct f2fs_sb_info *sbi, u32 crc,
			      const void *address, unsigned int length)
{
	return __f2fs_crc32(sbi, crc, address, length);
}