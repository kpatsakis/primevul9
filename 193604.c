int gg_chat_send_message(struct gg_session *gs, uint64_t id, const char *message, int is_html)
{
	return gg_send_message_110(gs, 0, id, message, is_html);
}