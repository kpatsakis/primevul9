static struct dentry *proc_fd_instantiate(struct inode *dir,
	struct dentry *dentry, struct task_struct *task, const void *ptr)
{
	unsigned fd = *(const unsigned *)ptr;
	struct file *file;
	struct files_struct *files;
 	struct inode *inode;
 	struct proc_inode *ei;
	struct dentry *error = ERR_PTR(-ENOENT);

	inode = proc_pid_make_inode(dir->i_sb, task);
	if (!inode)
		goto out;
	ei = PROC_I(inode);
	ei->fd = fd;
	files = get_files_struct(task);
	if (!files)
		goto out_iput;
	inode->i_mode = S_IFLNK;

	/*
	 * We are not taking a ref to the file structure, so we must
	 * hold ->file_lock.
	 */
	spin_lock(&files->file_lock);
	file = fcheck_files(files, fd);
	if (!file)
		goto out_unlock;
	if (file->f_mode & 1)
		inode->i_mode |= S_IRUSR | S_IXUSR;
	if (file->f_mode & 2)
		inode->i_mode |= S_IWUSR | S_IXUSR;
	spin_unlock(&files->file_lock);
	put_files_struct(files);

	inode->i_op = &proc_pid_link_inode_operations;
	inode->i_size = 64;
	ei->op.proc_get_link = proc_fd_link;
	dentry->d_op = &tid_fd_dentry_operations;
	d_add(dentry, inode);
	/* Close the race of the process dying before we return the dentry */
	if (tid_fd_revalidate(dentry, NULL))
		error = NULL;

 out:
	return error;
out_unlock:
	spin_unlock(&files->file_lock);
	put_files_struct(files);
out_iput:
	iput(inode);
	goto out;
}