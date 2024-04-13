static int chmod_common(struct path *path, umode_t mode)
{
	struct inode *inode = path->dentry->d_inode;
	struct iattr newattrs;
	int error;

	error = mnt_want_write(path->mnt);
	if (error)
		return error;
	mutex_lock(&inode->i_mutex);
	error = security_path_chmod(path, mode);
	if (error)
		goto out_unlock;
	newattrs.ia_mode = (mode & S_IALLUGO) | (inode->i_mode & ~S_IALLUGO);
	newattrs.ia_valid = ATTR_MODE | ATTR_CTIME;
	error = notify_change(path->dentry, &newattrs);
out_unlock:
	mutex_unlock(&inode->i_mutex);
	mnt_drop_write(path->mnt);
	return error;
}