int gg_send_message(struct gg_session *sess, int msgclass, uin_t recipient, const unsigned char *message)
{
	gg_debug_session(sess, GG_DEBUG_FUNCTION, "** gg_send_message(%p, %d, %u, %p)\n", sess, msgclass, recipient, message);

	return gg_send_message_common(sess, msgclass, 1, &recipient, message, (const unsigned char*) "\x02\x06\x00\x00\x00\x08\x00\x00\x00", 9, NULL);
}