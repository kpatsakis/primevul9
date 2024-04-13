static int btrfs_ioctl_set_fslabel(struct file *file, void __user *arg)
{
	struct inode *inode = file_inode(file);
	struct btrfs_fs_info *fs_info = btrfs_sb(inode->i_sb);
	struct btrfs_root *root = BTRFS_I(inode)->root;
	struct btrfs_super_block *super_block = fs_info->super_copy;
	struct btrfs_trans_handle *trans;
	char label[BTRFS_LABEL_SIZE];
	int ret;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	if (copy_from_user(label, arg, sizeof(label)))
		return -EFAULT;

	if (strnlen(label, BTRFS_LABEL_SIZE) == BTRFS_LABEL_SIZE) {
		btrfs_err(fs_info,
			  "unable to set label with more than %d bytes",
			  BTRFS_LABEL_SIZE - 1);
		return -EINVAL;
	}

	ret = mnt_want_write_file(file);
	if (ret)
		return ret;

	trans = btrfs_start_transaction(root, 0);
	if (IS_ERR(trans)) {
		ret = PTR_ERR(trans);
		goto out_unlock;
	}

	spin_lock(&fs_info->super_lock);
	strcpy(super_block->label, label);
	spin_unlock(&fs_info->super_lock);
	ret = btrfs_commit_transaction(trans);

out_unlock:
	mnt_drop_write_file(file);
	return ret;
}