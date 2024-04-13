static int coolkey_match_card(sc_card_t *card)
{
	int r;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);
	/* Since we send an APDU, the card's logout function may be called...
	 * however it may be in dirty memory */
	card->ops->logout = NULL;

	r = coolkey_select_applet(card);
	if (r == SC_SUCCESS) {
		sc_apdu_t apdu;

		/* The GET STATUS INS with P1 = 1 returns invalid instruction (0x6D00)
		 * on Coolkey applet (reserved for GetMemory function),
		 * while incorrect P1 (0x9C10) on Muscle applets
		 */
		sc_format_apdu(card, &apdu, SC_APDU_CASE_1, COOLKEY_INS_GET_STATUS, 0x01, 0x00);
		apdu.cla = COOLKEY_CLASS;
		apdu.le = 0x00;
		apdu.resplen = 0;
		apdu.resp = NULL;
		r = sc_transmit_apdu(card, &apdu);
		if (r == SC_SUCCESS && apdu.sw1 == 0x6d && apdu.sw2 == 0x00) {
			return 1;
		}
		return 0;
	}
	return 0;
}