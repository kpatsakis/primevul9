static int vfs_rename_other(struct inode *old_dir, struct dentry *old_dentry,
			    struct inode *new_dir, struct dentry *new_dentry)
{
	struct inode *target = new_dentry->d_inode;
	int error;

	error = security_inode_rename(old_dir, old_dentry, new_dir, new_dentry);
	if (error)
		return error;

	dget(new_dentry);
	if (target)
		mutex_lock(&target->i_mutex);

	error = -EBUSY;
	if (d_mountpoint(old_dentry)||d_mountpoint(new_dentry))
		goto out;

	error = old_dir->i_op->rename(old_dir, old_dentry, new_dir, new_dentry);
	if (error)
		goto out;

	if (target)
		dont_mount(new_dentry);
	if (!(old_dir->i_sb->s_type->fs_flags & FS_RENAME_DOES_D_MOVE))
		d_move(old_dentry, new_dentry);
out:
	if (target)
		mutex_unlock(&target->i_mutex);
	dput(new_dentry);
	return error;
}