int gg_send_message_richtext(struct gg_session *sess, int msgclass, uin_t recipient, const unsigned char *message, const unsigned char *format, int formatlen)
{
	gg_debug_session(sess, GG_DEBUG_FUNCTION, "** gg_send_message_richtext(%p, %d, %u, %p, %p, %d);\n", sess, msgclass, recipient, message, format, formatlen);

	return gg_send_message_common(sess, msgclass, 1, &recipient, message, format, formatlen, NULL);
}