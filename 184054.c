smtp_proceed_connected(struct smtp_session *s)
{
	if (s->listener->flags & F_SMTPS)
		smtp_cert_init(s);
	else
		smtp_send_banner(s);
}