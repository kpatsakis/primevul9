static long btrfs_ioctl_set_received_subvol(struct file *file,
					    void __user *arg)
{
	struct btrfs_ioctl_received_subvol_args *sa = NULL;
	int ret = 0;

	sa = memdup_user(arg, sizeof(*sa));
	if (IS_ERR(sa))
		return PTR_ERR(sa);

	ret = _btrfs_ioctl_set_received_subvol(file, sa);

	if (ret)
		goto out;

	ret = copy_to_user(arg, sa, sizeof(*sa));
	if (ret)
		ret = -EFAULT;

out:
	kfree(sa);
	return ret;
}