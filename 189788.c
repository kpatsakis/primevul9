static void *proc_self_follow_link(struct dentry *dentry, struct nameidata *nd)
{
	char tmp[PROC_NUMBUF];
	sprintf(tmp, "%d", task_tgid_vnr(current));
	return ERR_PTR(vfs_follow_link(nd,tmp));
}