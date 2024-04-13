smtp_send_banner(struct smtp_session *s)
{
	smtp_reply(s, "220 %s ESMTP %s", s->smtpname, SMTPD_NAME);
	s->banner_sent = 1;
	smtp_report_link_greeting(s, s->smtpname);
}