static long btrfs_ioctl_rm_dev(struct file *file, void __user *arg)
{
	struct inode *inode = file_inode(file);
	struct btrfs_fs_info *fs_info = btrfs_sb(inode->i_sb);
	struct btrfs_ioctl_vol_args *vol_args;
	int ret;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	ret = mnt_want_write_file(file);
	if (ret)
		return ret;

	if (test_and_set_bit(BTRFS_FS_EXCL_OP, &fs_info->flags)) {
		ret = BTRFS_ERROR_DEV_EXCL_RUN_IN_PROGRESS;
		goto out_drop_write;
	}

	vol_args = memdup_user(arg, sizeof(*vol_args));
	if (IS_ERR(vol_args)) {
		ret = PTR_ERR(vol_args);
		goto out;
	}

	vol_args->name[BTRFS_PATH_NAME_MAX] = '\0';
	ret = btrfs_rm_device(fs_info, vol_args->name, 0);

	if (!ret)
		btrfs_info(fs_info, "disk deleted %s", vol_args->name);
	kfree(vol_args);
out:
	clear_bit(BTRFS_FS_EXCL_OP, &fs_info->flags);
out_drop_write:
	mnt_drop_write_file(file);

	return ret;
}