vcs_fasync(int fd, struct file *file, int on)
{
	struct vcs_poll_data *poll = file->private_data;

	if (!poll) {
		/* don't allocate anything if all we want is disable fasync */
		if (!on)
			return 0;
		poll = vcs_poll_data_get(file);
		if (!poll)
			return -ENOMEM;
	}

	return fasync_helper(fd, file, on, &poll->fasync);
}