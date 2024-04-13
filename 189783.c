static int mountstats_open(struct inode *inode, struct file *file)
{
	int ret = seq_open(file, &mountstats_op);

	if (!ret) {
		struct seq_file *m = file->private_data;
		struct nsproxy *nsp;
		struct mnt_namespace *mnt_ns = NULL;
		struct task_struct *task = get_proc_task(inode);

		if (task) {
			rcu_read_lock();
			nsp = task_nsproxy(task);
			if (nsp) {
				mnt_ns = nsp->mnt_ns;
				if (mnt_ns)
					get_mnt_ns(mnt_ns);
			}
			rcu_read_unlock();

			put_task_struct(task);
		}

		if (mnt_ns)
			m->private = mnt_ns;
		else {
			seq_release(inode, file);
			ret = -EINVAL;
		}
	}
	return ret;
}