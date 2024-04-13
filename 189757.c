static int proc_self_readlink(struct dentry *dentry, char __user *buffer,
			      int buflen)
{
	char tmp[PROC_NUMBUF];
	sprintf(tmp, "%d", task_tgid_vnr(current));
	return vfs_readlink(dentry,buffer,buflen,tmp);
}