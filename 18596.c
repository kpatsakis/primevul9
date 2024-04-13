int __close_range(unsigned fd, unsigned max_fd, unsigned int flags)
{
	struct task_struct *me = current;
	struct files_struct *cur_fds = me->files, *fds = NULL;

	if (flags & ~(CLOSE_RANGE_UNSHARE | CLOSE_RANGE_CLOEXEC))
		return -EINVAL;

	if (fd > max_fd)
		return -EINVAL;

	if (flags & CLOSE_RANGE_UNSHARE) {
		int ret;
		unsigned int max_unshare_fds = NR_OPEN_MAX;

		/*
		 * If the caller requested all fds to be made cloexec we always
		 * copy all of the file descriptors since they still want to
		 * use them.
		 */
		if (!(flags & CLOSE_RANGE_CLOEXEC)) {
			/*
			 * If the requested range is greater than the current
			 * maximum, we're closing everything so only copy all
			 * file descriptors beneath the lowest file descriptor.
			 */
			rcu_read_lock();
			if (max_fd >= last_fd(files_fdtable(cur_fds)))
				max_unshare_fds = fd;
			rcu_read_unlock();
		}

		ret = unshare_fd(CLONE_FILES, max_unshare_fds, &fds);
		if (ret)
			return ret;

		/*
		 * We used to share our file descriptor table, and have now
		 * created a private one, make sure we're using it below.
		 */
		if (fds)
			swap(cur_fds, fds);
	}

	if (flags & CLOSE_RANGE_CLOEXEC)
		__range_cloexec(cur_fds, fd, max_fd);
	else
		__range_close(cur_fds, fd, max_fd);

	if (fds) {
		/*
		 * We're done closing the files we were supposed to. Time to install
		 * the new file descriptor table and drop the old one.
		 */
		task_lock(me);
		me->files = cur_fds;
		task_unlock(me);
		put_files_struct(fds);
	}

	return 0;
}