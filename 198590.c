static int muscle_match_card(sc_card_t *card)
{
	sc_apdu_t apdu;
	u8 response[64];
	int r;

	/* Since we send an APDU, the card's logout function may be called...
	 * however it's not always properly nulled out... */
	card->ops->logout = NULL;

	if (msc_select_applet(card, muscleAppletId, sizeof muscleAppletId) == 1) {
		/* Muscle applet is present, check the protocol version to be sure */
		sc_format_apdu(card, &apdu, SC_APDU_CASE_2, 0x3C, 0x00, 0x00);
		apdu.cla = 0xB0;
		apdu.le = 64;
		apdu.resplen = 64;
		apdu.resp = response;
		r = sc_transmit_apdu(card, &apdu);
		if (r == SC_SUCCESS && response[0] == 0x01) {
			card->type = SC_CARD_TYPE_MUSCLE_V1;
		} else {
			card->type = SC_CARD_TYPE_MUSCLE_GENERIC;
		}
		return 1;
	}
	return 0;
}