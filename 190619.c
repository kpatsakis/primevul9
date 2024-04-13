static inline unsigned long f2fs_qf_ino(struct super_block *sb, int type)
{
	if (type < F2FS_MAX_QUOTAS)
		return le32_to_cpu(F2FS_SB(sb)->raw_super->qf_ino[type]);
	return 0;
}