static struct fdtable *close_files(struct files_struct * files)
{
	/*
	 * It is safe to dereference the fd table without RCU or
	 * ->file_lock because this is the last reference to the
	 * files structure.
	 */
	struct fdtable *fdt = rcu_dereference_raw(files->fdt);
	unsigned int i, j = 0;

	for (;;) {
		unsigned long set;
		i = j * BITS_PER_LONG;
		if (i >= fdt->max_fds)
			break;
		set = fdt->open_fds[j++];
		while (set) {
			if (set & 1) {
				struct file * file = xchg(&fdt->fd[i], NULL);
				if (file) {
					filp_close(file, files);
					cond_resched();
				}
			}
			i++;
			set >>= 1;
		}
	}

	return fdt;
}