int gg_change_status_flags(struct gg_session *sess, int flags)
{
	gg_debug_session(sess, GG_DEBUG_FUNCTION, "** gg_change_status_flags(%p, 0x%08x);\n", sess, flags);

	if (sess == NULL) {
		errno = EFAULT;
		return -1;
	}

	sess->status_flags = flags;

	return 0;
}