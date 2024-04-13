static long btrfs_ioctl_rm_dev_v2(struct file *file, void __user *arg)
{
	struct inode *inode = file_inode(file);
	struct btrfs_fs_info *fs_info = btrfs_sb(inode->i_sb);
	struct btrfs_ioctl_vol_args_v2 *vol_args;
	int ret;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	ret = mnt_want_write_file(file);
	if (ret)
		return ret;

	vol_args = memdup_user(arg, sizeof(*vol_args));
	if (IS_ERR(vol_args)) {
		ret = PTR_ERR(vol_args);
		goto err_drop;
	}

	/* Check for compatibility reject unknown flags */
	if (vol_args->flags & ~BTRFS_VOL_ARG_V2_FLAGS_SUPPORTED) {
		ret = -EOPNOTSUPP;
		goto out;
	}

	if (test_and_set_bit(BTRFS_FS_EXCL_OP, &fs_info->flags)) {
		ret = BTRFS_ERROR_DEV_EXCL_RUN_IN_PROGRESS;
		goto out;
	}

	if (vol_args->flags & BTRFS_DEVICE_SPEC_BY_ID) {
		ret = btrfs_rm_device(fs_info, NULL, vol_args->devid);
	} else {
		vol_args->name[BTRFS_SUBVOL_NAME_MAX] = '\0';
		ret = btrfs_rm_device(fs_info, vol_args->name, 0);
	}
	clear_bit(BTRFS_FS_EXCL_OP, &fs_info->flags);

	if (!ret) {
		if (vol_args->flags & BTRFS_DEVICE_SPEC_BY_ID)
			btrfs_info(fs_info, "device deleted: id %llu",
					vol_args->devid);
		else
			btrfs_info(fs_info, "device deleted: %s",
					vol_args->name);
	}
out:
	kfree(vol_args);
err_drop:
	mnt_drop_write_file(file);
	return ret;
}