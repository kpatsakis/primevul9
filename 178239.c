static struct dentry *proc_root_lookup(struct inode * dir, struct dentry * dentry, unsigned int flags)
{
	if (!proc_pid_lookup(dir, dentry, flags))
		return NULL;
	
	return proc_lookup(dir, dentry, flags);
}