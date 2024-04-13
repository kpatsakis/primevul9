static int sc_pkcs15emu_sc_hsm_read_tokeninfo (sc_pkcs15_card_t * p15card)
{
	sc_card_t *card = p15card->card;
	int r;
	u8 efbin[512];
	size_t len;

	LOG_FUNC_CALLED(card->ctx);

	/* Read token info */
	len = sizeof efbin;
	r = read_file(p15card, (u8 *) "\x2F\x03", efbin, &len, 1);
	LOG_TEST_RET(card->ctx, r, "Skipping optional EF.TokenInfo");

	r = sc_pkcs15_parse_tokeninfo(card->ctx, p15card->tokeninfo, efbin, len);
	LOG_TEST_RET(card->ctx, r, "Skipping optional EF.TokenInfo");

	LOG_FUNC_RETURN(card->ctx, SC_SUCCESS);
}