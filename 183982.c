smtp_tx(struct smtp_session *s)
{
	struct smtp_tx *tx;

	tx = calloc(1, sizeof(*tx));
	if (tx == NULL)
		return 0;

	TAILQ_INIT(&tx->rcpts);

	s->tx = tx;
	tx->session = s;

	/* setup the envelope */
	tx->evp.ss = s->ss;
	(void)strlcpy(tx->evp.tag, s->listener->tag, sizeof(tx->evp.tag));
	(void)strlcpy(tx->evp.smtpname, s->smtpname, sizeof(tx->evp.smtpname));
	(void)strlcpy(tx->evp.hostname, s->rdns, sizeof tx->evp.hostname);
	(void)strlcpy(tx->evp.helo, s->helo, sizeof(tx->evp.helo));
	(void)strlcpy(tx->evp.username, s->username, sizeof(tx->evp.username));

	if (s->flags & SF_BOUNCE)
		tx->evp.flags |= EF_BOUNCE;
	if (s->flags & SF_AUTHENTICATED)
		tx->evp.flags |= EF_AUTHENTICATED;

	if ((tx->parser = rfc5322_parser_new()) == NULL) {
		free(tx);
		return 0;
	}

	return 1;
}