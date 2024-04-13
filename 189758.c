static struct fs_struct *get_fs_struct(struct task_struct *task)
{
	struct fs_struct *fs;
	task_lock(task);
	fs = task->fs;
	if(fs)
		atomic_inc(&fs->count);
	task_unlock(task);
	return fs;
}