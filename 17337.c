struct file *do_file_open_root(struct dentry *dentry, struct vfsmount *mnt,
		const char *name, const struct open_flags *op)
{
	struct nameidata nd;
	struct file *file;
	struct filename filename = { .name = name };
	int flags = op->lookup_flags | LOOKUP_ROOT;

	nd.root.mnt = mnt;
	nd.root.dentry = dentry;

	if (dentry->d_inode->i_op->follow_link && op->intent & LOOKUP_OPEN)
		return ERR_PTR(-ELOOP);

	file = path_openat(-1, &filename, &nd, op, flags | LOOKUP_RCU);
	if (unlikely(file == ERR_PTR(-ECHILD)))
		file = path_openat(-1, &filename, &nd, op, flags);
	if (unlikely(file == ERR_PTR(-ESTALE)))
		file = path_openat(-1, &filename, &nd, op, flags | LOOKUP_REVAL);
	return file;
}