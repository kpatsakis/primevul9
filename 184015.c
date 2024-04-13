smtp_filter_fd(struct smtp_tx *tx, int fd)
{
	struct smtp_session *s;

	s = tx->session;

	log_debug("smtp: %p: filter fd %d", s, fd);

	tx->filter = io_new();
	io_set_fd(tx->filter, fd);
	io_set_callback(tx->filter, filter_session_io, tx);
}