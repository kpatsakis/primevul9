int sc_pkcs15emu_gemsafeGPK_init_ex(sc_pkcs15_card_t *p15card, struct sc_aid *aid)
{
	sc_card_t   *card = p15card->card;
	sc_context_t    *ctx = card->ctx;

	sc_log(ctx,  "Entering %s", __FUNCTION__);

	if (gemsafe_detect_card(p15card))
		return SC_ERROR_WRONG_CARD;
	return sc_pkcs15emu_gemsafeGPK_init(p15card);
}