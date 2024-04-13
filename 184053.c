smtp_report_protocol_server(struct smtp_session *s, const char *response)
{
	if (! SESSION_FILTERED(s))
		return;

	report_smtp_protocol_server("smtp-in", s->id, response);
}