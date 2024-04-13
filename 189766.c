static struct dentry *proc_base_instantiate(struct inode *dir,
	struct dentry *dentry, struct task_struct *task, const void *ptr)
{
	const struct pid_entry *p = ptr;
	struct inode *inode;
	struct proc_inode *ei;
	struct dentry *error = ERR_PTR(-EINVAL);

	/* Allocate the inode */
	error = ERR_PTR(-ENOMEM);
	inode = new_inode(dir->i_sb);
	if (!inode)
		goto out;

	/* Initialize the inode */
	ei = PROC_I(inode);
	inode->i_mtime = inode->i_atime = inode->i_ctime = CURRENT_TIME;

	/*
	 * grab the reference to the task.
	 */
	ei->pid = get_task_pid(task, PIDTYPE_PID);
	if (!ei->pid)
		goto out_iput;

	inode->i_uid = 0;
	inode->i_gid = 0;
	inode->i_mode = p->mode;
	if (S_ISDIR(inode->i_mode))
		inode->i_nlink = 2;
	if (S_ISLNK(inode->i_mode))
		inode->i_size = 64;
	if (p->iop)
		inode->i_op = p->iop;
	if (p->fop)
		inode->i_fop = p->fop;
	ei->op = p->op;
	dentry->d_op = &proc_base_dentry_operations;
	d_add(dentry, inode);
	error = NULL;
out:
	return error;
out_iput:
	iput(inode);
	goto out;
}