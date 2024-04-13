smtp_cert_verify(struct smtp_session *s)
{
	const char *name;
	int fallback;

	if (s->listener->ca_name[0]) {
		name = s->listener->ca_name;
		fallback = 0;
	}
	else {
		name = s->smtpname;
		fallback = 1;
	}

	if (cert_verify(io_tls(s->io), name, fallback, smtp_cert_verify_cb, s)) {
		tree_xset(&wait_ssl_verify, s->id, s);
		io_pause(s->io, IO_IN);
	}
}