}

static inline bool f2fs_hw_support_discard(struct f2fs_sb_info *sbi)
{
	int i;

	if (!f2fs_is_multi_device(sbi))
		return f2fs_bdev_support_discard(sbi->sb->s_bdev);

	for (i = 0; i < sbi->s_ndevs; i++)
		if (f2fs_bdev_support_discard(FDEV(i).bdev))
			return true;