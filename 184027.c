smtp_report_tx_commit(struct smtp_session *s, uint32_t msgid, size_t msgsz)
{
	if (! SESSION_FILTERED(s))
		return;

	report_smtp_tx_commit("smtp-in", s->id, msgid, msgsz);
}