smtp_report_link_identify(struct smtp_session *s, const char *method, const char *identity)
{
	if (! SESSION_FILTERED(s))
		return;

	report_smtp_link_identify("smtp-in", s->id, method, identity);
}