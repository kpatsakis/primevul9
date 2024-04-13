static int cac_get_ACA_path(sc_card_t *card, sc_path_t *path)
{
	cac_private_data_t * priv = CAC_DATA(card);

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_NORMAL);
	if (priv->aca_path) {
		*path = *priv->aca_path;
	}
	SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_NORMAL, SC_SUCCESS);
}