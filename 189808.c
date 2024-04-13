static int proc_fd_link(struct inode *inode, struct dentry **dentry,
			struct vfsmount **mnt)
{
	return proc_fd_info(inode, dentry, mnt, NULL);
}