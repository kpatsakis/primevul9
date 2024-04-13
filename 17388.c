static int vfs_rename_dir(struct inode *old_dir, struct dentry *old_dentry,
			  struct inode *new_dir, struct dentry *new_dentry)
{
	int error = 0;
	struct inode *target = new_dentry->d_inode;
	unsigned max_links = new_dir->i_sb->s_max_links;

	/*
	 * If we are going to change the parent - check write permissions,
	 * we'll need to flip '..'.
	 */
	if (new_dir != old_dir) {
		error = inode_permission(old_dentry->d_inode, MAY_WRITE);
		if (error)
			return error;
	}

	error = security_inode_rename(old_dir, old_dentry, new_dir, new_dentry);
	if (error)
		return error;

	dget(new_dentry);
	if (target)
		mutex_lock(&target->i_mutex);

	error = -EBUSY;
	if (d_mountpoint(old_dentry) || d_mountpoint(new_dentry))
		goto out;

	error = -EMLINK;
	if (max_links && !target && new_dir != old_dir &&
	    new_dir->i_nlink >= max_links)
		goto out;

	if (target)
		shrink_dcache_parent(new_dentry);
	error = old_dir->i_op->rename(old_dir, old_dentry, new_dir, new_dentry);
	if (error)
		goto out;

	if (target) {
		target->i_flags |= S_DEAD;
		dont_mount(new_dentry);
	}
out:
	if (target)
		mutex_unlock(&target->i_mutex);
	dput(new_dentry);
	if (!error)
		if (!(old_dir->i_sb->s_type->fs_flags & FS_RENAME_DOES_D_MOVE))
			d_move(old_dentry,new_dentry);
	return error;
}