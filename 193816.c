static int setcos_list_files(sc_card_t *card, u8 * buf, size_t buflen)
{
	sc_apdu_t apdu;
	int r;

	sc_format_apdu(card, &apdu, SC_APDU_CASE_2_SHORT, 0xAA, 0, 0);
	if (card->type == SC_CARD_TYPE_SETCOS_44 || 
	    card->type == SC_CARD_TYPE_SETCOS_NIDEL ||
	    SETCOS_IS_EID_APPLET(card))
		apdu.cla = 0x80;
	apdu.resp = buf;
	apdu.resplen = buflen;
	apdu.le = buflen > 256 ? 256 : buflen;
	r = sc_transmit_apdu(card, &apdu);
	LOG_TEST_RET(card->ctx, r, "APDU transmit failed");
	if (card->type == SC_CARD_TYPE_SETCOS_44 && apdu.sw1 == 0x6A && apdu.sw2 == 0x82)
		return 0; /* no files found */
	if (apdu.resplen == 0)
		return sc_check_sw(card, apdu.sw1, apdu.sw2);
	return apdu.resplen;
}