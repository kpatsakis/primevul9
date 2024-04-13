static int btrfs_ioctl_get_fslabel(struct file *file, void __user *arg)
{
	struct inode *inode = file_inode(file);
	struct btrfs_fs_info *fs_info = btrfs_sb(inode->i_sb);
	size_t len;
	int ret;
	char label[BTRFS_LABEL_SIZE];

	spin_lock(&fs_info->super_lock);
	memcpy(label, fs_info->super_copy->label, BTRFS_LABEL_SIZE);
	spin_unlock(&fs_info->super_lock);

	len = strnlen(label, BTRFS_LABEL_SIZE);

	if (len == BTRFS_LABEL_SIZE) {
		btrfs_warn(fs_info,
			   "label is too long, return the first %zu bytes",
			   --len);
	}

	ret = copy_to_user(arg, label, len);

	return ret ? -EFAULT : 0;
}