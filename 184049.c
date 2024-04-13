smtp_report_tx_mail(struct smtp_session *s, uint32_t msgid, const char *address, int ok)
{
	char	mailaddr[SMTPD_MAXMAILADDRSIZE];
	char    *p;

	if (! SESSION_FILTERED(s))
		return;

	if ((p = strchr(address, '<')) == NULL)
		return;
	(void)strlcpy(mailaddr, p + 1, sizeof mailaddr);
	if ((p = strchr(mailaddr, '>')) == NULL)
		return;
	*p = '\0';

	report_smtp_tx_mail("smtp-in", s->id, msgid, mailaddr, ok);
}