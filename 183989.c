smtp_report_tx_envelope(struct smtp_session *s, uint32_t msgid, uint64_t evpid)
{
	if (! SESSION_FILTERED(s))
		return;

	report_smtp_tx_envelope("smtp-in", s->id, msgid, evpid);
}