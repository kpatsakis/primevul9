struct file *file_open_root(struct dentry *dentry, struct vfsmount *mnt,
			    const char *filename, int flags)
{
	struct open_flags op;
	int err = build_open_flags(flags, 0, &op);
	if (err)
		return ERR_PTR(err);
	if (flags & O_CREAT)
		return ERR_PTR(-EINVAL);
	if (!filename && (flags & O_DIRECTORY))
		if (!dentry->d_inode->i_op->lookup)
			return ERR_PTR(-ENOTDIR);
	return do_file_open_root(dentry, mnt, filename, &op);
}