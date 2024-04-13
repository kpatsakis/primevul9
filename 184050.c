smtp_cert_verify_cb(void *arg, int status)
{
	struct smtp_session *s = arg;
	const char *reason = NULL;
	int resume;

	resume = tree_pop(&wait_ssl_verify, s->id) != NULL;

	switch (status) {
	case CERT_OK:
		reason = "cert-ok";
		s->flags |= SF_VERIFIED;
		break;
	case CERT_NOCA:
		reason = "no-ca";
		break;
	case CERT_NOCERT:
		reason = "no-client-cert";
		break;
	case CERT_INVALID:
		reason = "cert-invalid";
		break;
	default:
		reason = "cert-check-failed";
		break;
	}

	log_debug("smtp: %p: smtp_cert_verify_cb: %s", s, reason);

	if (!(s->flags & SF_VERIFIED) && (s->listener->flags & F_TLS_VERIFY)) {
		log_info("%016"PRIx64" smtp disconnected "
		    " reason=%s", s->id,
		    reason);
		smtp_free(s, "SSL certificate check failed");
		return;
	}

	smtp_tls_verified(s);
	if (resume)
		io_resume(s->io, IO_IN);
}