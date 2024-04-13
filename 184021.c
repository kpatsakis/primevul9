smtp_message_fd(struct smtp_tx *tx, int fd)
{
	struct smtp_session *s;

	s = tx->session;

	log_debug("smtp: %p: message fd %d", s, fd);

	if ((tx->ofile = fdopen(fd, "w")) == NULL) {
		close(fd);
		smtp_reply(s, "421 %s Temporary Error",
		    esc_code(ESC_STATUS_TEMPFAIL, ESC_OTHER_MAIL_SYSTEM_STATUS));
		smtp_enter_state(s, STATE_QUIT);
		return 0;
	}
	return 1;
}