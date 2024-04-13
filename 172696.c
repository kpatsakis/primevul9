static noinline_for_stack int scrub_supers(struct scrub_ctx *sctx,
					   struct btrfs_device *scrub_dev)
{
	int	i;
	u64	bytenr;
	u64	gen;
	int	ret;
	struct btrfs_fs_info *fs_info = sctx->fs_info;

	if (test_bit(BTRFS_FS_STATE_ERROR, &fs_info->fs_state))
		return -EIO;

	/* Seed devices of a new filesystem has their own generation. */
	if (scrub_dev->fs_devices != fs_info->fs_devices)
		gen = scrub_dev->generation;
	else
		gen = fs_info->last_trans_committed;

	for (i = 0; i < BTRFS_SUPER_MIRROR_MAX; i++) {
		bytenr = btrfs_sb_offset(i);
		if (bytenr + BTRFS_SUPER_INFO_SIZE >
		    scrub_dev->commit_total_bytes)
			break;

		ret = scrub_pages(sctx, bytenr, BTRFS_SUPER_INFO_SIZE, bytenr,
				  scrub_dev, BTRFS_EXTENT_FLAG_SUPER, gen, i,
				  NULL, 1, bytenr);
		if (ret)
			return ret;
	}
	wait_event(sctx->list_wait, atomic_read(&sctx->bios_in_flight) == 0);

	return 0;
}