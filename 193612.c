int gg_send_message_confer_html(struct gg_session *sess, int msgclass, int recipients_count, uin_t *recipients, const unsigned char *html_message)
{
	gg_debug_session(sess, GG_DEBUG_FUNCTION, "** gg_send_message_confer_html(%p, %d, %d, %p, %p);\n", sess, msgclass, recipients_count, recipients, html_message);

	return gg_send_message_common(sess, msgclass, recipients_count, recipients, NULL, NULL, 0, html_message);
}