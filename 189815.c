static int lstats_open(struct inode *inode, struct file *file)
{
	int ret;
	struct seq_file *m;
	struct task_struct *task = get_proc_task(inode);

	ret = single_open(file, lstats_show_proc, NULL);
	if (!ret) {
		m = file->private_data;
		m->private = task;
	}
	return ret;
}