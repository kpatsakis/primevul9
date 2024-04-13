static inline u32 f2fs_crc32(struct f2fs_sb_info *sbi, const void *address,
			   unsigned int length)
{
	return __f2fs_crc32(sbi, F2FS_SUPER_MAGIC, address, length);
}