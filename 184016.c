smtp_report_filter_response(struct smtp_session *s, int phase, int response, const char *param)
{
	if (! SESSION_FILTERED(s))
		return;

	report_smtp_filter_response("smtp-in", s->id, phase, response, param);
}