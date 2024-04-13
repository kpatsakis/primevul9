smtp_proceed_data(struct smtp_session *s, const char *args)
{
	smtp_tx_open_message(s->tx);
}