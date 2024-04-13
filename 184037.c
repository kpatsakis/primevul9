smtp_report_tx_begin(struct smtp_session *s, uint32_t msgid)
{
	if (! SESSION_FILTERED(s))
		return;

	report_smtp_tx_begin("smtp-in", s->id, msgid);
}