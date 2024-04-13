static int cac_process_ACA(sc_card_t *card, cac_private_data_t *priv)
{
	int r;
	u8 *val = NULL;
	size_t val_len;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);

	/* Assuming ACA is already selected */
	r = cac_get_acr(card, CAC_ACR_SERVICE, &val, &val_len);
	if (r < 0)
		goto done;

	r = cac_parse_ACA_service(card, priv, val, val_len);
        if (r == SC_SUCCESS) {
		priv->aca_path = malloc(sizeof(sc_path_t));
		if (!priv->aca_path) {
			r = SC_ERROR_OUT_OF_MEMORY;
			goto done;
		}
		memcpy(priv->aca_path, &cac_ACA_Path, sizeof(sc_path_t));
	}
done:
	if (val)
		free(val);
	SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_NORMAL, r);
}