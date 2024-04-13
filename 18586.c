	__acquires(files->file_lock)
{
	struct fdtable *fdt;
	int expanded = 0;

repeat:
	fdt = files_fdtable(files);

	/* Do we need to expand? */
	if (nr < fdt->max_fds)
		return expanded;

	/* Can we expand? */
	if (nr >= sysctl_nr_open)
		return -EMFILE;

	if (unlikely(files->resize_in_progress)) {
		spin_unlock(&files->file_lock);
		expanded = 1;
		wait_event(files->resize_wait, !files->resize_in_progress);
		spin_lock(&files->file_lock);
		goto repeat;
	}

	/* All good, so we try */
	files->resize_in_progress = true;
	expanded = expand_fdtable(files, nr);
	files->resize_in_progress = false;

	wake_up_all(&files->resize_wait);
	return expanded;
}