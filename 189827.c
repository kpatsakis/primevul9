static struct dentry *proc_pident_instantiate(struct inode *dir,
	struct dentry *dentry, struct task_struct *task, const void *ptr)
{
	const struct pid_entry *p = ptr;
	struct inode *inode;
	struct proc_inode *ei;
	struct dentry *error = ERR_PTR(-EINVAL);

	inode = proc_pid_make_inode(dir->i_sb, task);
	if (!inode)
		goto out;

	ei = PROC_I(inode);
	inode->i_mode = p->mode;
	if (S_ISDIR(inode->i_mode))
		inode->i_nlink = 2;	/* Use getattr to fix if necessary */
	if (p->iop)
		inode->i_op = p->iop;
	if (p->fop)
		inode->i_fop = p->fop;
	ei->op = p->op;
	dentry->d_op = &pid_dentry_operations;
	d_add(dentry, inode);
	/* Close the race of the process dying before we return the dentry */
	if (pid_revalidate(dentry, NULL))
		error = NULL;
out:
	return error;
}