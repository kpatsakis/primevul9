int gg_send_message_confer(struct gg_session *sess, int msgclass, int recipients_count, uin_t *recipients, const unsigned char *message)
{
	gg_debug_session(sess, GG_DEBUG_FUNCTION, "** gg_send_message_confer(%p, %d, %d, %p, %p);\n", sess, msgclass, recipients_count, recipients, message);

	return gg_send_message_common(sess, msgclass, recipients_count, recipients, message, (const unsigned char*) "\x02\x06\x00\x00\x00\x08\x00\x00\x00", 9, NULL);
}