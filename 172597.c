void btrfs_set_fs_info_ptr(struct btrfs_fs_info *fs_info)
{
	struct btrfs_fs_devices *fs_devices = fs_info->fs_devices;
	while (fs_devices) {
		fs_devices->fs_info = fs_info;
		fs_devices = fs_devices->seed;
	}
}