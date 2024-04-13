smtp_proceed_rset(struct smtp_session *s, const char *args)
{
	smtp_reply(s, "250 %s Reset state",
	    esc_code(ESC_STATUS_OK, ESC_OTHER_STATUS));

	if (s->tx) {
		if (s->tx->msgid)
			smtp_tx_rollback(s->tx);
		smtp_tx_free(s->tx);
	}
}