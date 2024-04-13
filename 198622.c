epass2003_create_file(struct sc_card *card, sc_file_t * file)
{
	int r;
	size_t len;
	u8 sbuf[SC_MAX_APDU_BUFFER_SIZE] = { 0 };
	struct sc_apdu apdu;

	len = SC_MAX_APDU_BUFFER_SIZE;

	epass2003_hook_file(file, 1);

	if (card->ops->construct_fci == NULL)
		LOG_FUNC_RETURN(card->ctx, SC_ERROR_NOT_SUPPORTED);

	r = epass2003_construct_fci(card, file, sbuf, &len);
	LOG_TEST_RET(card->ctx, r, "construct_fci() failed");

	sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0xE0, 0x00, 0x00);
	apdu.lc = len;
	apdu.datalen = len;
	apdu.data = sbuf;

	r = sc_transmit_apdu_t(card, &apdu);
	LOG_TEST_RET(card->ctx, r, "APDU transmit failed");
	r = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(card->ctx, r, "APDU sw1/2 wrong");

	epass2003_hook_file(file, 0);
	return r;
}