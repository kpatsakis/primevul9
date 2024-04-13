static long btrfs_ioctl_add_dev(struct btrfs_fs_info *fs_info, void __user *arg)
{
	struct btrfs_ioctl_vol_args *vol_args;
	int ret;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	if (test_and_set_bit(BTRFS_FS_EXCL_OP, &fs_info->flags))
		return BTRFS_ERROR_DEV_EXCL_RUN_IN_PROGRESS;

	vol_args = memdup_user(arg, sizeof(*vol_args));
	if (IS_ERR(vol_args)) {
		ret = PTR_ERR(vol_args);
		goto out;
	}

	vol_args->name[BTRFS_PATH_NAME_MAX] = '\0';
	ret = btrfs_init_new_device(fs_info, vol_args->name);

	if (!ret)
		btrfs_info(fs_info, "disk added %s", vol_args->name);

	kfree(vol_args);
out:
	clear_bit(BTRFS_FS_EXCL_OP, &fs_info->flags);
	return ret;
}