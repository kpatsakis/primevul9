static struct dentry *proc_fdinfo_instantiate(struct inode *dir,
	struct dentry *dentry, struct task_struct *task, const void *ptr)
{
	unsigned fd = *(unsigned *)ptr;
 	struct inode *inode;
 	struct proc_inode *ei;
	struct dentry *error = ERR_PTR(-ENOENT);

	inode = proc_pid_make_inode(dir->i_sb, task);
	if (!inode)
		goto out;
	ei = PROC_I(inode);
	ei->fd = fd;
	inode->i_mode = S_IFREG | S_IRUSR;
	inode->i_fop = &proc_fdinfo_file_operations;
	dentry->d_op = &tid_fd_dentry_operations;
	d_add(dentry, inode);
	/* Close the race of the process dying before we return the dentry */
	if (tid_fd_revalidate(dentry, NULL))
		error = NULL;

 out:
	return error;
}