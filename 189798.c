static int proc_fd_permission(struct inode *inode, int mask,
				struct nameidata *nd)
{
	int rv;

	rv = generic_permission(inode, mask, NULL);
	if (rv == 0)
		return 0;
	if (task_pid(current) == proc_pid(inode))
		rv = 0;
	return rv;
}