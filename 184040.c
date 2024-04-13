smtp_tx_free(struct smtp_tx *tx)
{
	struct smtp_rcpt *rcpt;

	rfc5322_free(tx->parser);

	while ((rcpt = TAILQ_FIRST(&tx->rcpts))) {
		TAILQ_REMOVE(&tx->rcpts, rcpt, entry);
		free(rcpt);
	}

	if (tx->ofile)
		fclose(tx->ofile);

	tx->session->tx = NULL;

	free(tx);
}