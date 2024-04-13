static noinline int btrfs_mksubvol(const struct path *parent,
				   const char *name, int namelen,
				   struct btrfs_root *snap_src,
				   u64 *async_transid, bool readonly,
				   struct btrfs_qgroup_inherit *inherit)
{
	struct inode *dir = d_inode(parent->dentry);
	struct btrfs_fs_info *fs_info = btrfs_sb(dir->i_sb);
	struct dentry *dentry;
	int error;

	error = down_write_killable_nested(&dir->i_rwsem, I_MUTEX_PARENT);
	if (error == -EINTR)
		return error;

	dentry = lookup_one_len(name, parent->dentry, namelen);
	error = PTR_ERR(dentry);
	if (IS_ERR(dentry))
		goto out_unlock;

	error = btrfs_may_create(dir, dentry);
	if (error)
		goto out_dput;

	/*
	 * even if this name doesn't exist, we may get hash collisions.
	 * check for them now when we can safely fail
	 */
	error = btrfs_check_dir_item_collision(BTRFS_I(dir)->root,
					       dir->i_ino, name,
					       namelen);
	if (error)
		goto out_dput;

	down_read(&fs_info->subvol_sem);

	if (btrfs_root_refs(&BTRFS_I(dir)->root->root_item) == 0)
		goto out_up_read;

	if (snap_src) {
		error = create_snapshot(snap_src, dir, dentry,
					async_transid, readonly, inherit);
	} else {
		error = create_subvol(dir, dentry, name, namelen,
				      async_transid, inherit);
	}
	if (!error)
		fsnotify_mkdir(dir, dentry);
out_up_read:
	up_read(&fs_info->subvol_sem);
out_dput:
	dput(dentry);
out_unlock:
	inode_unlock(dir);
	return error;
}