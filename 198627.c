epass2003_refresh(struct sc_card *card)
{
	int r = SC_SUCCESS;
	epass2003_exdata *exdata = NULL;

	if (!card->drv_data) 
		return SC_ERROR_INVALID_ARGUMENTS;

	exdata = (epass2003_exdata *)card->drv_data;

	if (exdata->sm) {
		card->sm_ctx.sm_mode = 0;
		r = mutual_auth(card, g_init_key_enc, g_init_key_mac);
		card->sm_ctx.sm_mode = SM_MODE_TRANSMIT;
		LOG_TEST_RET(card->ctx, r, "mutual_auth failed");
	}

	return r;
}