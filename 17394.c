static int do_tmpfile(int dfd, struct filename *pathname,
		struct nameidata *nd, int flags,
		const struct open_flags *op,
		struct file *file, int *opened)
{
	static const struct qstr name = QSTR_INIT("/", 1);
	struct dentry *dentry, *child;
	struct inode *dir;
	int error = path_lookupat(dfd, pathname->name,
				  flags | LOOKUP_DIRECTORY, nd);
	if (unlikely(error))
		return error;
	error = mnt_want_write(nd->path.mnt);
	if (unlikely(error))
		goto out;
	/* we want directory to be writable */
	error = inode_permission(nd->inode, MAY_WRITE | MAY_EXEC);
	if (error)
		goto out2;
	dentry = nd->path.dentry;
	dir = dentry->d_inode;
	if (!dir->i_op->tmpfile) {
		error = -EOPNOTSUPP;
		goto out2;
	}
	child = d_alloc(dentry, &name);
	if (unlikely(!child)) {
		error = -ENOMEM;
		goto out2;
	}
	nd->flags &= ~LOOKUP_DIRECTORY;
	nd->flags |= op->intent;
	dput(nd->path.dentry);
	nd->path.dentry = child;
	error = dir->i_op->tmpfile(dir, nd->path.dentry, op->mode);
	if (error)
		goto out2;
	audit_inode(pathname, nd->path.dentry, 0);
	error = may_open(&nd->path, op->acc_mode, op->open_flag);
	if (error)
		goto out2;
	file->f_path.mnt = nd->path.mnt;
	error = finish_open(file, nd->path.dentry, NULL, opened);
	if (error)
		goto out2;
	error = open_check_o_direct(file);
	if (error) {
		fput(file);
	} else if (!(op->open_flag & O_EXCL)) {
		struct inode *inode = file_inode(file);
		spin_lock(&inode->i_lock);
		inode->i_state |= I_LINKABLE;
		spin_unlock(&inode->i_lock);
	}
out2:
	mnt_drop_write(nd->path.mnt);
out:
	path_put(&nd->path);
	return error;
}