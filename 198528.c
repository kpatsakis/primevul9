static int tcos_setperm(sc_card_t *card, int enable_nullpin)
{
	int r;
	sc_apdu_t apdu;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);
	sc_format_apdu(card, &apdu, SC_APDU_CASE_1, 0xEE, 0x00, 0x00);
        apdu.cla |= 0x80;
	apdu.lc = 0;
	apdu.datalen = 0;
	apdu.data = NULL;
	
	r = sc_transmit_apdu(card, &apdu);
	SC_TEST_RET(card->ctx, SC_LOG_DEBUG_NORMAL, r, "APDU transmit failed");
	return sc_check_sw(card, apdu.sw1, apdu.sw2);
}