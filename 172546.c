static noinline int btrfs_ioctl_subvol_setflags(struct file *file,
					      void __user *arg)
{
	struct inode *inode = file_inode(file);
	struct btrfs_fs_info *fs_info = btrfs_sb(inode->i_sb);
	struct btrfs_root *root = BTRFS_I(inode)->root;
	struct btrfs_trans_handle *trans;
	u64 root_flags;
	u64 flags;
	int ret = 0;

	if (!inode_owner_or_capable(inode))
		return -EPERM;

	ret = mnt_want_write_file(file);
	if (ret)
		goto out;

	if (btrfs_ino(BTRFS_I(inode)) != BTRFS_FIRST_FREE_OBJECTID) {
		ret = -EINVAL;
		goto out_drop_write;
	}

	if (copy_from_user(&flags, arg, sizeof(flags))) {
		ret = -EFAULT;
		goto out_drop_write;
	}

	if (flags & BTRFS_SUBVOL_CREATE_ASYNC) {
		ret = -EINVAL;
		goto out_drop_write;
	}

	if (flags & ~BTRFS_SUBVOL_RDONLY) {
		ret = -EOPNOTSUPP;
		goto out_drop_write;
	}

	down_write(&fs_info->subvol_sem);

	/* nothing to do */
	if (!!(flags & BTRFS_SUBVOL_RDONLY) == btrfs_root_readonly(root))
		goto out_drop_sem;

	root_flags = btrfs_root_flags(&root->root_item);
	if (flags & BTRFS_SUBVOL_RDONLY) {
		btrfs_set_root_flags(&root->root_item,
				     root_flags | BTRFS_ROOT_SUBVOL_RDONLY);
	} else {
		/*
		 * Block RO -> RW transition if this subvolume is involved in
		 * send
		 */
		spin_lock(&root->root_item_lock);
		if (root->send_in_progress == 0) {
			btrfs_set_root_flags(&root->root_item,
				     root_flags & ~BTRFS_ROOT_SUBVOL_RDONLY);
			spin_unlock(&root->root_item_lock);
		} else {
			spin_unlock(&root->root_item_lock);
			btrfs_warn(fs_info,
				   "Attempt to set subvolume %llu read-write during send",
				   root->root_key.objectid);
			ret = -EPERM;
			goto out_drop_sem;
		}
	}

	trans = btrfs_start_transaction(root, 1);
	if (IS_ERR(trans)) {
		ret = PTR_ERR(trans);
		goto out_reset;
	}

	ret = btrfs_update_root(trans, fs_info->tree_root,
				&root->root_key, &root->root_item);
	if (ret < 0) {
		btrfs_end_transaction(trans);
		goto out_reset;
	}

	ret = btrfs_commit_transaction(trans);

out_reset:
	if (ret)
		btrfs_set_root_flags(&root->root_item, root_flags);
out_drop_sem:
	up_write(&fs_info->subvol_sem);
out_drop_write:
	mnt_drop_write_file(file);
out:
	return ret;
}