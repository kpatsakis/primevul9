epass2003_list_files(struct sc_card *card, unsigned char *buf, size_t buflen)
{
	struct sc_apdu apdu;
	unsigned char rbuf[SC_MAX_APDU_BUFFER_SIZE] = { 0 };
	int r;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);
	sc_format_apdu(card, &apdu, SC_APDU_CASE_1, 0x34, 0x00, 0x00);
	apdu.cla = 0x80;
	apdu.le = 0;
	apdu.resplen = sizeof(rbuf);
	apdu.resp = rbuf;

	r = sc_transmit_apdu_t(card, &apdu);
	LOG_TEST_RET(card->ctx, r, "APDU transmit failed");

	r = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(card->ctx, r, "Card returned error");

	if (apdu.resplen == 0x100 && rbuf[0] == 0 && rbuf[1] == 0)
		LOG_FUNC_RETURN(card->ctx, 0);

	buflen = buflen < apdu.resplen ? buflen : apdu.resplen;
	memcpy(buf, rbuf, buflen);

	LOG_FUNC_RETURN(card->ctx, buflen);
}