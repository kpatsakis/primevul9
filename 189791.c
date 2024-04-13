static int pid_delete_dentry(struct dentry * dentry)
{
	/* Is the task we represent dead?
	 * If so, then don't put the dentry on the lru list,
	 * kill it immediately.
	 */
	return !proc_pid(dentry->d_inode)->tasks[PIDTYPE_PID].first;
}