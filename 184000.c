smtp_proceed_rollback(struct smtp_session *s, const char *args)
{
	struct smtp_tx *tx;

	tx = s->tx;

	fclose(tx->ofile);
	tx->ofile = NULL;

	smtp_tx_rollback(tx);
	smtp_tx_free(tx);
	smtp_enter_state(s, STATE_HELO);
}