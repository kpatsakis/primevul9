void gg_logoff(struct gg_session *sess)
{
	if (!sess)
		return;

	gg_debug_session(sess, GG_DEBUG_FUNCTION, "** gg_logoff(%p);\n", sess);

#ifdef GG_CONFIG_HAVE_GNUTLS
	if (sess->ssl != NULL)
		gnutls_bye(GG_SESSION_GNUTLS(sess), GNUTLS_SHUT_RDWR);
#endif

#ifdef GG_CONFIG_HAVE_OPENSSL
	if (sess->ssl != NULL)
		SSL_shutdown(sess->ssl);
#endif

	sess->resolver_cleanup(&sess->resolver, 1);

	if (sess->fd != -1) {
		close(sess->fd);
		sess->fd = -1;
	}

	if (sess->send_buf) {
		free(sess->send_buf);
		sess->send_buf = NULL;
		sess->send_left = 0;
	}
}