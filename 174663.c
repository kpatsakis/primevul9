vcs_poll_data_get(struct file *file)
{
	struct vcs_poll_data *poll = file->private_data, *kill = NULL;

	if (poll)
		return poll;

	poll = kzalloc(sizeof(*poll), GFP_KERNEL);
	if (!poll)
		return NULL;
	poll->cons_num = console(file_inode(file));
	init_waitqueue_head(&poll->waitq);
	poll->notifier.notifier_call = vcs_notifier;
	/*
	 * In order not to lose any update event, we must pretend one might
	 * have occurred before we have a chance to register our notifier.
	 * This is also how user space has come to detect which kernels
	 * support POLLPRI on /dev/vcs* devices i.e. using poll() with
	 * POLLPRI and a zero timeout.
	 */
	poll->event = VT_UPDATE;

	if (register_vt_notifier(&poll->notifier) != 0) {
		kfree(poll);
		return NULL;
	}

	/*
	 * This code may be called either through ->poll() or ->fasync().
	 * If we have two threads using the same file descriptor, they could
	 * both enter this function, both notice that the structure hasn't
	 * been allocated yet and go ahead allocating it in parallel, but
	 * only one of them must survive and be shared otherwise we'd leak
	 * memory with a dangling notifier callback.
	 */
	spin_lock(&file->f_lock);
	if (!file->private_data) {
		file->private_data = poll;
	} else {
		/* someone else raced ahead of us */
		kill = poll;
		poll = file->private_data;
	}
	spin_unlock(&file->f_lock);
	if (kill)
		vcs_poll_data_free(kill);

	return poll;
}