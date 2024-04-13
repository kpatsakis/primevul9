void gg_free_session(struct gg_session *sess)
{
	struct gg_dcc7 *dcc;
	struct gg_chat_list *chat;

	gg_debug_session(sess, GG_DEBUG_FUNCTION, "** gg_free_session(%p);\n", sess);

	if (sess == NULL)
		return;

	/* XXX dopisać zwalnianie i zamykanie wszystkiego, co mogło zostać */

	free(sess->resolver_result);
	free(sess->connect_host);
	free(sess->password);
	free(sess->initial_descr);
	free(sess->client_version);
	free(sess->header_buf);
	free(sess->recv_buf);

#ifdef GG_CONFIG_HAVE_GNUTLS
	if (sess->ssl != NULL) {
		gg_session_gnutls_t *tmp;

		tmp = (gg_session_gnutls_t*) sess->ssl;
		gnutls_deinit(tmp->session);
		gnutls_certificate_free_credentials(tmp->xcred);
		gnutls_global_deinit();
		free(sess->ssl);
	}
#endif

#ifdef GG_CONFIG_HAVE_OPENSSL
	if (sess->ssl)
		SSL_free(sess->ssl);

	if (sess->ssl_ctx)
		SSL_CTX_free(sess->ssl_ctx);
#endif

	sess->resolver_cleanup(&sess->resolver, 1);

	if (sess->fd != -1)
		close(sess->fd);

	while (sess->images)
		gg_image_queue_remove(sess, sess->images, 1);

	free(sess->send_buf);

	for (dcc = sess->dcc7_list; dcc; dcc = dcc->next)
		dcc->sess = NULL;

	chat = sess->chat_list;
	while (chat != NULL) {
		struct gg_chat_list *next = chat->next;
		free(chat->participants);
		free(chat);
		chat = next;
	}

	free(sess);
}