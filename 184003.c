smtp_report_link_auth(struct smtp_session *s, const char *user, const char *result)
{
	if (! SESSION_FILTERED(s))
		return;

	report_smtp_link_auth("smtp-in", s->id, user, result);
}