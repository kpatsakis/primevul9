vcs_poll(struct file *file, poll_table *wait)
{
	struct vcs_poll_data *poll = vcs_poll_data_get(file);
	__poll_t ret = DEFAULT_POLLMASK|EPOLLERR;

	if (poll) {
		poll_wait(file, &poll->waitq, wait);
		switch (poll->event) {
		case VT_UPDATE:
			ret = DEFAULT_POLLMASK|EPOLLPRI;
			break;
		case VT_DEALLOCATE:
			ret = DEFAULT_POLLMASK|EPOLLHUP|EPOLLERR;
			break;
		case 0:
			ret = DEFAULT_POLLMASK;
			break;
		}
	}
	return ret;
}