int gg_ping(struct gg_session *sess)
{
	gg_debug_session(sess, GG_DEBUG_FUNCTION, "** gg_ping(%p);\n", sess);

	if (!sess) {
		errno = EFAULT;
		return -1;
	}

	if (sess->state != GG_STATE_CONNECTED) {
		errno = ENOTCONN;
		return -1;
	}

	return gg_send_packet(sess, GG_PING, NULL);
}