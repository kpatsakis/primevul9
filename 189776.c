static int mounts_open(struct inode *inode, struct file *file)
{
	struct task_struct *task = get_proc_task(inode);
	struct nsproxy *nsp;
	struct mnt_namespace *ns = NULL;
	struct proc_mounts *p;
	int ret = -EINVAL;

	if (task) {
		rcu_read_lock();
		nsp = task_nsproxy(task);
		if (nsp) {
			ns = nsp->mnt_ns;
			if (ns)
				get_mnt_ns(ns);
		}
		rcu_read_unlock();

		put_task_struct(task);
	}

	if (ns) {
		ret = -ENOMEM;
		p = kmalloc(sizeof(struct proc_mounts), GFP_KERNEL);
		if (p) {
			file->private_data = &p->m;
			ret = seq_open(file, &mounts_op);
			if (!ret) {
				p->m.private = ns;
				p->event = ns->event;
				return 0;
			}
			kfree(p);
		}
		put_mnt_ns(ns);
	}
	return ret;
}