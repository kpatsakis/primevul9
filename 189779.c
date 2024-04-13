static int sched_show(struct seq_file *m, void *v)
{
	struct inode *inode = m->private;
	struct task_struct *p;

	WARN_ON(!inode);

	p = get_proc_task(inode);
	if (!p)
		return -ESRCH;
	proc_sched_show_task(p, m);

	put_task_struct(p);

	return 0;
}