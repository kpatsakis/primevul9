smtp_tls_verified(struct smtp_session *s)
{
	X509 *x;

	x = SSL_get_peer_certificate(io_tls(s->io));
	if (x) {
		log_info("%016"PRIx64" smtp "
		    "client-cert-check result=\"%s\"",
		    s->id,
		    (s->flags & SF_VERIFIED) ? "success" : "failure");
		X509_free(x);
	}

	if (s->listener->flags & F_SMTPS) {
		stat_increment("smtp.smtps", 1);
		io_set_write(s->io);
		smtp_send_banner(s);
	}
	else {
		stat_increment("smtp.tls", 1);
		smtp_enter_state(s, STATE_HELO);
	}
}