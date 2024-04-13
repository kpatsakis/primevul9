static int tcos_create_file(sc_card_t *card, sc_file_t *file)
{
	int r;
	size_t len;
	u8 sbuf[SC_MAX_APDU_BUFFER_SIZE];
	sc_apdu_t apdu;

	len = SC_MAX_APDU_BUFFER_SIZE;
	r = tcos_construct_fci(file, sbuf, &len);
	SC_TEST_RET(card->ctx, SC_LOG_DEBUG_NORMAL, r, "tcos_construct_fci() failed");
	
	sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0xE0, 0x00, 0x00);
        apdu.cla |= 0x80;  /* this is an proprietary extension */
	apdu.lc = len;
	apdu.datalen = len;
	apdu.data = sbuf;

	r = sc_transmit_apdu(card, &apdu);
	SC_TEST_RET(card->ctx, SC_LOG_DEBUG_NORMAL, r, "APDU transmit failed");
	return sc_check_sw(card, apdu.sw1, apdu.sw2);
}