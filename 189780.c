static ssize_t lstats_write(struct file *file, const char __user *buf,
			    size_t count, loff_t *offs)
{
	struct seq_file *m;
	struct task_struct *task;

	m = file->private_data;
	task = m->private;
	clear_all_latency_tracing(task);

	return count;
}