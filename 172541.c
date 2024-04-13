static int btrfs_ioctl_fssetxattr(struct file *file, void __user *arg)
{
	struct inode *inode = file_inode(file);
	struct btrfs_inode *binode = BTRFS_I(inode);
	struct btrfs_root *root = binode->root;
	struct btrfs_trans_handle *trans;
	struct fsxattr fa;
	unsigned old_flags;
	unsigned old_i_flags;
	int ret = 0;

	if (!inode_owner_or_capable(inode))
		return -EPERM;

	if (btrfs_root_readonly(root))
		return -EROFS;

	memset(&fa, 0, sizeof(fa));
	if (copy_from_user(&fa, arg, sizeof(fa)))
		return -EFAULT;

	ret = check_xflags(fa.fsx_xflags);
	if (ret)
		return ret;

	if (fa.fsx_extsize != 0 || fa.fsx_projid != 0 || fa.fsx_cowextsize != 0)
		return -EOPNOTSUPP;

	ret = mnt_want_write_file(file);
	if (ret)
		return ret;

	inode_lock(inode);

	old_flags = binode->flags;
	old_i_flags = inode->i_flags;

	/* We need the capabilities to change append-only or immutable inode */
	if (((old_flags & (BTRFS_INODE_APPEND | BTRFS_INODE_IMMUTABLE)) ||
	     (fa.fsx_xflags & (FS_XFLAG_APPEND | FS_XFLAG_IMMUTABLE))) &&
	    !capable(CAP_LINUX_IMMUTABLE)) {
		ret = -EPERM;
		goto out_unlock;
	}

	if (fa.fsx_xflags & FS_XFLAG_SYNC)
		binode->flags |= BTRFS_INODE_SYNC;
	else
		binode->flags &= ~BTRFS_INODE_SYNC;
	if (fa.fsx_xflags & FS_XFLAG_IMMUTABLE)
		binode->flags |= BTRFS_INODE_IMMUTABLE;
	else
		binode->flags &= ~BTRFS_INODE_IMMUTABLE;
	if (fa.fsx_xflags & FS_XFLAG_APPEND)
		binode->flags |= BTRFS_INODE_APPEND;
	else
		binode->flags &= ~BTRFS_INODE_APPEND;
	if (fa.fsx_xflags & FS_XFLAG_NODUMP)
		binode->flags |= BTRFS_INODE_NODUMP;
	else
		binode->flags &= ~BTRFS_INODE_NODUMP;
	if (fa.fsx_xflags & FS_XFLAG_NOATIME)
		binode->flags |= BTRFS_INODE_NOATIME;
	else
		binode->flags &= ~BTRFS_INODE_NOATIME;

	/* 1 item for the inode */
	trans = btrfs_start_transaction(root, 1);
	if (IS_ERR(trans)) {
		ret = PTR_ERR(trans);
		goto out_unlock;
	}

	btrfs_sync_inode_flags_to_i_flags(inode);
	inode_inc_iversion(inode);
	inode->i_ctime = current_time(inode);
	ret = btrfs_update_inode(trans, root, inode);

	btrfs_end_transaction(trans);

out_unlock:
	if (ret) {
		binode->flags = old_flags;
		inode->i_flags = old_i_flags;
	}

	inode_unlock(inode);
	mnt_drop_write_file(file);

	return ret;
}