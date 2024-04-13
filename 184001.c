smtp_free(struct smtp_session *s, const char * reason)
{
	if (s->tx) {
		if (s->tx->msgid)
			smtp_tx_rollback(s->tx);
		smtp_tx_free(s->tx);
	}

	smtp_report_link_disconnect(s);
	smtp_filter_end(s);

	if (s->flags & SF_SECURE && s->listener->flags & F_SMTPS)
		stat_decrement("smtp.smtps", 1);
	if (s->flags & SF_SECURE && s->listener->flags & F_STARTTLS)
		stat_decrement("smtp.tls", 1);

	io_free(s->io);
	free(s);

	smtp_collect();
}