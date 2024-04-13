static int proc_task_getattr(struct vfsmount *mnt, struct dentry *dentry, struct kstat *stat)
{
	struct inode *inode = dentry->d_inode;
	struct task_struct *p = get_proc_task(inode);
	generic_fillattr(inode, stat);

	if (p) {
		rcu_read_lock();
		stat->nlink += get_nr_threads(p);
		rcu_read_unlock();
		put_task_struct(p);
	}

	return 0;
}