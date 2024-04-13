}

static inline bool f2fs_hw_is_readonly(struct f2fs_sb_info *sbi)
{
	int i;

	if (!f2fs_is_multi_device(sbi))
		return bdev_read_only(sbi->sb->s_bdev);

	for (i = 0; i < sbi->s_ndevs; i++)
		if (bdev_read_only(FDEV(i).bdev))
			return true;