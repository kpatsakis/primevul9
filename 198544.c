get_external_key_retries(struct sc_card *card, unsigned char kid, unsigned char *retries)
{
	int r;
	struct sc_apdu apdu;
	unsigned char random[16] = { 0 };

	r = sc_get_challenge(card, random, 8);
	LOG_TEST_RET(card->ctx, r, "get challenge get_external_key_retries failed");

	sc_format_apdu(card, &apdu, SC_APDU_CASE_1, 0x82, 0x01, 0x80 | kid);
	apdu.resp = NULL;
	apdu.resplen = 0;

	r = sc_transmit_apdu_t(card, &apdu);
	LOG_TEST_RET(card->ctx, r, "APDU get_external_key_retries failed");

	if (retries && ((0x63 == (apdu.sw1 & 0xff)) && (0xC0 == (apdu.sw2 & 0xf0)))) {
		*retries = (apdu.sw2 & 0x0f);
		r = SC_SUCCESS;
	}
	else {
		LOG_TEST_RET(card->ctx, r, "get_external_key_retries failed");
		r = SC_ERROR_CARD_CMD_FAILED;
	}

	return r;
}