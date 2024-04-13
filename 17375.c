int vfs_rename(struct inode *old_dir, struct dentry *old_dentry,
	       struct inode *new_dir, struct dentry *new_dentry)
{
	int error;
	int is_dir = S_ISDIR(old_dentry->d_inode->i_mode);
	const unsigned char *old_name;

	if (old_dentry->d_inode == new_dentry->d_inode)
 		return 0;
 
	error = may_delete(old_dir, old_dentry, is_dir);
	if (error)
		return error;

	if (!new_dentry->d_inode)
		error = may_create(new_dir, new_dentry);
	else
		error = may_delete(new_dir, new_dentry, is_dir);
	if (error)
		return error;

	if (!old_dir->i_op->rename)
		return -EPERM;

	old_name = fsnotify_oldname_init(old_dentry->d_name.name);

	if (is_dir)
		error = vfs_rename_dir(old_dir,old_dentry,new_dir,new_dentry);
	else
		error = vfs_rename_other(old_dir,old_dentry,new_dir,new_dentry);
	if (!error)
		fsnotify_move(old_dir, new_dir, old_name, is_dir,
			      new_dentry->d_inode, old_dentry);
	fsnotify_oldname_free(old_name);

	return error;
}