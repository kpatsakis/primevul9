static int btrfs_ioctl_set_features(struct file *file, void __user *arg)
{
	struct inode *inode = file_inode(file);
	struct btrfs_fs_info *fs_info = btrfs_sb(inode->i_sb);
	struct btrfs_root *root = BTRFS_I(inode)->root;
	struct btrfs_super_block *super_block = fs_info->super_copy;
	struct btrfs_ioctl_feature_flags flags[2];
	struct btrfs_trans_handle *trans;
	u64 newflags;
	int ret;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	if (copy_from_user(flags, arg, sizeof(flags)))
		return -EFAULT;

	/* Nothing to do */
	if (!flags[0].compat_flags && !flags[0].compat_ro_flags &&
	    !flags[0].incompat_flags)
		return 0;

	ret = check_feature(fs_info, flags[0].compat_flags,
			    flags[1].compat_flags, COMPAT);
	if (ret)
		return ret;

	ret = check_feature(fs_info, flags[0].compat_ro_flags,
			    flags[1].compat_ro_flags, COMPAT_RO);
	if (ret)
		return ret;

	ret = check_feature(fs_info, flags[0].incompat_flags,
			    flags[1].incompat_flags, INCOMPAT);
	if (ret)
		return ret;

	ret = mnt_want_write_file(file);
	if (ret)
		return ret;

	trans = btrfs_start_transaction(root, 0);
	if (IS_ERR(trans)) {
		ret = PTR_ERR(trans);
		goto out_drop_write;
	}

	spin_lock(&fs_info->super_lock);
	newflags = btrfs_super_compat_flags(super_block);
	newflags |= flags[0].compat_flags & flags[1].compat_flags;
	newflags &= ~(flags[0].compat_flags & ~flags[1].compat_flags);
	btrfs_set_super_compat_flags(super_block, newflags);

	newflags = btrfs_super_compat_ro_flags(super_block);
	newflags |= flags[0].compat_ro_flags & flags[1].compat_ro_flags;
	newflags &= ~(flags[0].compat_ro_flags & ~flags[1].compat_ro_flags);
	btrfs_set_super_compat_ro_flags(super_block, newflags);

	newflags = btrfs_super_incompat_flags(super_block);
	newflags |= flags[0].incompat_flags & flags[1].incompat_flags;
	newflags &= ~(flags[0].incompat_flags & ~flags[1].incompat_flags);
	btrfs_set_super_incompat_flags(super_block, newflags);
	spin_unlock(&fs_info->super_lock);

	ret = btrfs_commit_transaction(trans);
out_drop_write:
	mnt_drop_write_file(file);

	return ret;
}