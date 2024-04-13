int gg_change_status_descr_time(struct gg_session *sess, int status, const char *descr, int ts)
{
	gg_debug_session(sess, GG_DEBUG_FUNCTION, "** gg_change_status_descr_time(%p, %d, \"%s\", %d);\n", sess, status, descr, ts);

	return gg_change_status_descr(sess, status, descr);
}