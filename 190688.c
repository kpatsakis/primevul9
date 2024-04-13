		<= (F2FS_OLD_ATTRIBUTE_SIZE + (extra_isize)))	\

static inline void f2fs_reset_iostat(struct f2fs_sb_info *sbi)
{
	int i;

	spin_lock(&sbi->iostat_lock);
	for (i = 0; i < NR_IO_TYPE; i++)
		sbi->write_iostat[i] = 0;