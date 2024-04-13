static int btrfs_check_raid_min_devices(struct btrfs_fs_info *fs_info,
		u64 num_devices)
{
	u64 all_avail;
	unsigned seq;
	int i;

	do {
		seq = read_seqbegin(&fs_info->profiles_lock);

		all_avail = fs_info->avail_data_alloc_bits |
			    fs_info->avail_system_alloc_bits |
			    fs_info->avail_metadata_alloc_bits;
	} while (read_seqretry(&fs_info->profiles_lock, seq));

	for (i = 0; i < BTRFS_NR_RAID_TYPES; i++) {
		if (!(all_avail & btrfs_raid_array[i].bg_flag))
			continue;

		if (num_devices < btrfs_raid_array[i].devs_min) {
			int ret = btrfs_raid_array[i].mindev_error;

			if (ret)
				return ret;
		}
	}

	return 0;
}