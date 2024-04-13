smtp_report_link_connect(struct smtp_session *s, const char *rdns, int fcrdns,
    const struct sockaddr_storage *ss_src,
    const struct sockaddr_storage *ss_dest)
{
	if (! SESSION_FILTERED(s))
		return;

	report_smtp_link_connect("smtp-in", s->id, rdns, fcrdns, ss_src, ss_dest);
}