static int proc_fd_access_allowed(struct inode *inode)
{
	struct task_struct *task;
	int allowed = 0;
	/* Allow access to a task's file descriptors if it is us or we
	 * may use ptrace attach to the process and find out that
	 * information.
	 */
	task = get_proc_task(inode);
	if (task) {
		allowed = ptrace_may_attach(task);
		put_task_struct(task);
	}
	return allowed;
}