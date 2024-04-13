static int show_numa_map_checked(struct seq_file *m, void *v)
{
	struct proc_maps_private *priv = m->private;
	struct task_struct *task = priv->task;

	if (maps_protect && !ptrace_may_attach(task))
		return -EACCES;

	return show_numa_map(m, v);
}