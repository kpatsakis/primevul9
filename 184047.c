smtp_cert_init(struct smtp_session *s)
{
	const char *name;
	int fallback;

	if (s->listener->pki_name[0]) {
		name = s->listener->pki_name;
		fallback = 0;
	}
	else {
		name = s->smtpname;
		fallback = 1;
	}

	if (cert_init(name, fallback, smtp_cert_init_cb, s))
		tree_xset(&wait_ssl_init, s->id, s);
}