smtp_proceed_commit(struct smtp_session *s, const char *args)
{
	smtp_message_end(s->tx);
}