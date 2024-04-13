smtp_report_tx_data(struct smtp_session *s, uint32_t msgid, int ok)
{
	if (! SESSION_FILTERED(s))
		return;

	report_smtp_tx_data("smtp-in", s->id, msgid, ok);
}