void fd_install(unsigned int fd, struct file *file)
{
	struct files_struct *files = current->files;
	struct fdtable *fdt;

	rcu_read_lock_sched();

	if (unlikely(files->resize_in_progress)) {
		rcu_read_unlock_sched();
		spin_lock(&files->file_lock);
		fdt = files_fdtable(files);
		BUG_ON(fdt->fd[fd] != NULL);
		rcu_assign_pointer(fdt->fd[fd], file);
		spin_unlock(&files->file_lock);
		return;
	}
	/* coupled with smp_wmb() in expand_fdtable() */
	smp_rmb();
	fdt = rcu_dereference_sched(files->fdt);
	BUG_ON(fdt->fd[fd] != NULL);
	rcu_assign_pointer(fdt->fd[fd], file);
	rcu_read_unlock_sched();
}