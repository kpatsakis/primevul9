smtp_cert_init_cb(void *arg, int status, const char *name, const void *cert,
    size_t cert_len)
{
	struct smtp_session *s = arg;
	void *ssl, *ssl_ctx;

	tree_pop(&wait_ssl_init, s->id);

	if (status == CA_FAIL) {
		log_info("%016"PRIx64" smtp disconnected "
		    "reason=ca-failure",
		    s->id);
		smtp_free(s, "CA failure");
		return;
	}

	ssl_ctx = dict_get(env->sc_ssl_dict, name);
	ssl = ssl_smtp_init(ssl_ctx, s->listener->flags & F_TLS_VERIFY);
	io_set_read(s->io);
	io_start_tls(s->io, ssl);
}