static int gg_session_callback(struct gg_session *sess)
{
	if (!sess) {
		errno = EFAULT;
		return -1;
	}

	return ((sess->event = gg_watch_fd(sess)) != NULL) ? 0 : -1;
}