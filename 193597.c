int gg_send_message_confer_richtext(struct gg_session *sess, int msgclass, int recipients_count, uin_t *recipients, const unsigned char *message, const unsigned char *format, int formatlen)
{
	gg_debug_session(sess, GG_DEBUG_FUNCTION, "** gg_send_message_confer_richtext(%p, %d, %d, %p, %p, %p, %d);\n", sess, msgclass, recipients_count, recipients, message, format, formatlen);

	return gg_send_message_common(sess, msgclass, recipients_count, recipients, message, format, formatlen, NULL);
}