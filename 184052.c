smtp_report_tx_rollback(struct smtp_session *s, uint32_t msgid)
{
	if (! SESSION_FILTERED(s))
		return;

	report_smtp_tx_rollback("smtp-in", s->id, msgid);
}