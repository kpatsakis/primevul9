get_data(struct sc_card *card, unsigned char type, unsigned char *data, size_t datalen)
{
	int r;
	struct sc_apdu apdu;
	unsigned char resp[SC_MAX_APDU_BUFFER_SIZE] = { 0 };
	size_t resplen = SC_MAX_APDU_BUFFER_SIZE;
	epass2003_exdata *exdata = NULL;

	if (!card->drv_data) 
		return SC_ERROR_INVALID_ARGUMENTS;

	exdata = (epass2003_exdata *)card->drv_data;

	LOG_FUNC_CALLED(card->ctx);

	sc_format_apdu(card, &apdu, SC_APDU_CASE_2_SHORT, 0xca, 0x01, type);
	apdu.resp = resp;
	apdu.le = 0;
	apdu.resplen = resplen;
	if (0x86 == type) {
		/* No SM temporarily */
		unsigned char tmp_sm = exdata->sm;
		exdata->sm = SM_PLAIN;
		r = sc_transmit_apdu(card, &apdu);
		exdata->sm = tmp_sm;
	}
	else {
		r = sc_transmit_apdu_t(card, &apdu);
	}
	LOG_TEST_RET(card->ctx, r, "APDU get_data failed");
	r = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(card->ctx, r, "get_data failed");

	memcpy(data, resp, datalen);
	return r;
}