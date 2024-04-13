int gg_send_message_html(struct gg_session *sess, int msgclass, uin_t recipient, const unsigned char *html_message)
{
	gg_debug_session(sess, GG_DEBUG_FUNCTION, "** gg_send_message_html(%p, %d, %u, %p);\n", sess, msgclass, recipient, html_message);

	return gg_send_message_common(sess, msgclass, 1, &recipient, NULL, NULL, 0, html_message);
}