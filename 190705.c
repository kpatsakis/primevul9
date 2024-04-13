static inline __u64 cur_cp_crc(struct f2fs_checkpoint *cp)
{
	size_t crc_offset = le32_to_cpu(cp->checksum_offset);
	return le32_to_cpu(*((__le32 *)((unsigned char *)cp + crc_offset)));
}