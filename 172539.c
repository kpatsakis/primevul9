static struct btrfs_fs_devices *open_seed_devices(struct btrfs_fs_info *fs_info,
						  u8 *fsid)
{
	struct btrfs_fs_devices *fs_devices;
	int ret;

	lockdep_assert_held(&uuid_mutex);
	ASSERT(fsid);

	fs_devices = fs_info->fs_devices->seed;
	while (fs_devices) {
		if (!memcmp(fs_devices->fsid, fsid, BTRFS_FSID_SIZE))
			return fs_devices;

		fs_devices = fs_devices->seed;
	}

	fs_devices = find_fsid(fsid, NULL);
	if (!fs_devices) {
		if (!btrfs_test_opt(fs_info, DEGRADED))
			return ERR_PTR(-ENOENT);

		fs_devices = alloc_fs_devices(fsid, NULL);
		if (IS_ERR(fs_devices))
			return fs_devices;

		fs_devices->seeding = 1;
		fs_devices->opened = 1;
		return fs_devices;
	}

	fs_devices = clone_fs_devices(fs_devices);
	if (IS_ERR(fs_devices))
		return fs_devices;

	ret = open_fs_devices(fs_devices, FMODE_READ, fs_info->bdev_holder);
	if (ret) {
		free_fs_devices(fs_devices);
		fs_devices = ERR_PTR(ret);
		goto out;
	}

	if (!fs_devices->seeding) {
		close_fs_devices(fs_devices);
		free_fs_devices(fs_devices);
		fs_devices = ERR_PTR(-EINVAL);
		goto out;
	}

	fs_devices->seed = fs_info->fs_devices->seed;
	fs_info->fs_devices->seed = fs_devices;
out:
	return fs_devices;
}