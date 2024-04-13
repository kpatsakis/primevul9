smtp_report_protocol_client(struct smtp_session *s, const char *command)
{
	if (! SESSION_FILTERED(s))
		return;

	report_smtp_protocol_client("smtp-in", s->id, command);
}