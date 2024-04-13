static long btrfs_ioctl_dev_replace(struct btrfs_fs_info *fs_info,
				    void __user *arg)
{
	struct btrfs_ioctl_dev_replace_args *p;
	int ret;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	p = memdup_user(arg, sizeof(*p));
	if (IS_ERR(p))
		return PTR_ERR(p);

	switch (p->cmd) {
	case BTRFS_IOCTL_DEV_REPLACE_CMD_START:
		if (sb_rdonly(fs_info->sb)) {
			ret = -EROFS;
			goto out;
		}
		if (test_and_set_bit(BTRFS_FS_EXCL_OP, &fs_info->flags)) {
			ret = BTRFS_ERROR_DEV_EXCL_RUN_IN_PROGRESS;
		} else {
			ret = btrfs_dev_replace_by_ioctl(fs_info, p);
			clear_bit(BTRFS_FS_EXCL_OP, &fs_info->flags);
		}
		break;
	case BTRFS_IOCTL_DEV_REPLACE_CMD_STATUS:
		btrfs_dev_replace_status(fs_info, p);
		ret = 0;
		break;
	case BTRFS_IOCTL_DEV_REPLACE_CMD_CANCEL:
		p->result = btrfs_dev_replace_cancel(fs_info);
		ret = 0;
		break;
	default:
		ret = -EINVAL;
		break;
	}

	if ((ret == 0 || ret == -ECANCELED) && copy_to_user(arg, p, sizeof(*p)))
		ret = -EFAULT;
out:
	kfree(p);
	return ret;
}