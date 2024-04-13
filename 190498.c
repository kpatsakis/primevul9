int f2fs_target_device_index(struct f2fs_sb_info *sbi, block_t blkaddr)
{
	int i;

	if (!f2fs_is_multi_device(sbi))
		return 0;

	for (i = 0; i < sbi->s_ndevs; i++)
		if (FDEV(i).start_blk <= blkaddr && FDEV(i).end_blk >= blkaddr)
			return i;
	return 0;
}