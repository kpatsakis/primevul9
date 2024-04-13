int gg_change_status(struct gg_session *sess, int status)
{
	gg_debug_session(sess, GG_DEBUG_FUNCTION, "** gg_change_status(%p, %d);\n", sess, status);

	return gg_change_status_descr(sess, status, NULL);
}