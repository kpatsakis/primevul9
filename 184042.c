smtp_proceed_rcpt_to(struct smtp_session *s, const char *args)
{
	smtp_tx_rcpt_to(s->tx, args);
}