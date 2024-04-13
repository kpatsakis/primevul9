}

static inline void f2fs_update_iostat(struct f2fs_sb_info *sbi,
			enum iostat_type type, unsigned long long io_bytes)
{
	if (!sbi->iostat_enable)
		return;
	spin_lock(&sbi->iostat_lock);
	sbi->write_iostat[type] += io_bytes;

	if (type == APP_WRITE_IO || type == APP_DIRECT_IO)
		sbi->write_iostat[APP_BUFFERED_IO] =
			sbi->write_iostat[APP_WRITE_IO] -
			sbi->write_iostat[APP_DIRECT_IO];