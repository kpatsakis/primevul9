epass2003_transmit_apdu(struct sc_card *card, struct sc_apdu *apdu)
{
	int r;

	LOG_FUNC_CALLED(card->ctx);

	r = sc_transmit_apdu_t(card, apdu);
	LOG_TEST_RET(card->ctx, r, "APDU transmit failed");

	return r;
}