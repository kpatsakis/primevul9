internal_write_rsa_key_factor(struct sc_card *card, unsigned short fid, u8 factor,
		sc_pkcs15_bignum_t data)
{
	int r;
	struct sc_apdu apdu;
	u8 sbuff[SC_MAX_EXT_APDU_BUFFER_SIZE] = { 0 };

	LOG_FUNC_CALLED(card->ctx);

	sbuff[0] = ((fid & 0xff00) >> 8);
	sbuff[1] = (fid & 0x00ff);
	memcpy(&sbuff[2], data.data, data.len);
//	sc_mem_reverse(&sbuff[2], data.len);

	sc_format_apdu(card, &apdu, SC_APDU_CASE_3, 0xe7, factor, 0x00);
	apdu.cla = 0x80;
	apdu.lc = apdu.datalen = 2 + data.len;
	apdu.data = sbuff;

	r = sc_transmit_apdu_t(card, &apdu);
	LOG_TEST_RET(card->ctx, r, "APDU transmit failed");
	r = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(card->ctx, r, "Write rsa key factor failed");

	LOG_FUNC_RETURN(card->ctx, SC_SUCCESS);
}