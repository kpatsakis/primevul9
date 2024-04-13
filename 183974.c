smtp_tx_eom(struct smtp_tx *tx)
{
	smtp_filter_phase(FILTER_COMMIT, tx->session, NULL);
}