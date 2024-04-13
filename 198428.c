mutual_auth(struct sc_card *card, unsigned char *key_enc,
			unsigned char *key_mac)
{
	struct sc_context *ctx = card->ctx;
	int r;
	unsigned char result[256] = { 0 };
	unsigned char ran_key[8] = { 0 };
	epass2003_exdata *exdata = NULL;

	if (!card->drv_data) 
		return SC_ERROR_INVALID_ARGUMENTS;
	exdata = (epass2003_exdata *)card->drv_data;

	LOG_FUNC_CALLED(ctx);

	r = gen_init_key(card, key_enc, key_mac, result, exdata->smtype);
	LOG_TEST_RET(ctx, r, "gen_init_key failed");
	memcpy(ran_key, &result[12], 8);

	r = verify_init_key(card, ran_key, exdata->smtype);
	LOG_TEST_RET(ctx, r, "verify_init_key failed");

	LOG_FUNC_RETURN(ctx, r);
}