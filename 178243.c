static int proc_root_getattr(struct vfsmount *mnt, struct dentry *dentry, struct kstat *stat
)
{
	generic_fillattr(d_inode(dentry), stat);
	stat->nlink = proc_root.nlink + nr_processes();
	return 0;
}