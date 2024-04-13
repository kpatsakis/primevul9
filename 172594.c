static noinline int btrfs_ioctl_snap_destroy(struct file *file,
					     void __user *arg)
{
	struct dentry *parent = file->f_path.dentry;
	struct btrfs_fs_info *fs_info = btrfs_sb(parent->d_sb);
	struct dentry *dentry;
	struct inode *dir = d_inode(parent);
	struct inode *inode;
	struct btrfs_root *root = BTRFS_I(dir)->root;
	struct btrfs_root *dest = NULL;
	struct btrfs_ioctl_vol_args *vol_args;
	int namelen;
	int err = 0;

	if (!S_ISDIR(dir->i_mode))
		return -ENOTDIR;

	vol_args = memdup_user(arg, sizeof(*vol_args));
	if (IS_ERR(vol_args))
		return PTR_ERR(vol_args);

	vol_args->name[BTRFS_PATH_NAME_MAX] = '\0';
	namelen = strlen(vol_args->name);
	if (strchr(vol_args->name, '/') ||
	    strncmp(vol_args->name, "..", namelen) == 0) {
		err = -EINVAL;
		goto out;
	}

	err = mnt_want_write_file(file);
	if (err)
		goto out;


	err = down_write_killable_nested(&dir->i_rwsem, I_MUTEX_PARENT);
	if (err == -EINTR)
		goto out_drop_write;
	dentry = lookup_one_len(vol_args->name, parent, namelen);
	if (IS_ERR(dentry)) {
		err = PTR_ERR(dentry);
		goto out_unlock_dir;
	}

	if (d_really_is_negative(dentry)) {
		err = -ENOENT;
		goto out_dput;
	}

	inode = d_inode(dentry);
	dest = BTRFS_I(inode)->root;
	if (!capable(CAP_SYS_ADMIN)) {
		/*
		 * Regular user.  Only allow this with a special mount
		 * option, when the user has write+exec access to the
		 * subvol root, and when rmdir(2) would have been
		 * allowed.
		 *
		 * Note that this is _not_ check that the subvol is
		 * empty or doesn't contain data that we wouldn't
		 * otherwise be able to delete.
		 *
		 * Users who want to delete empty subvols should try
		 * rmdir(2).
		 */
		err = -EPERM;
		if (!btrfs_test_opt(fs_info, USER_SUBVOL_RM_ALLOWED))
			goto out_dput;

		/*
		 * Do not allow deletion if the parent dir is the same
		 * as the dir to be deleted.  That means the ioctl
		 * must be called on the dentry referencing the root
		 * of the subvol, not a random directory contained
		 * within it.
		 */
		err = -EINVAL;
		if (root == dest)
			goto out_dput;

		err = inode_permission(inode, MAY_WRITE | MAY_EXEC);
		if (err)
			goto out_dput;
	}

	/* check if subvolume may be deleted by a user */
	err = btrfs_may_delete(dir, dentry, 1);
	if (err)
		goto out_dput;

	if (btrfs_ino(BTRFS_I(inode)) != BTRFS_FIRST_FREE_OBJECTID) {
		err = -EINVAL;
		goto out_dput;
	}

	inode_lock(inode);
	err = btrfs_delete_subvolume(dir, dentry);
	inode_unlock(inode);
	if (!err)
		d_delete(dentry);

out_dput:
	dput(dentry);
out_unlock_dir:
	inode_unlock(dir);
out_drop_write:
	mnt_drop_write_file(file);
out:
	kfree(vol_args);
	return err;
}