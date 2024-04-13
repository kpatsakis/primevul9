sc_transmit_apdu_t(sc_card_t *card, sc_apdu_t *apdu)
{
	int r = sc_transmit_apdu(card, apdu);
	if ( ((0x69 == apdu->sw1) && (0x85 == apdu->sw2)) || ((0x69 == apdu->sw1) && (0x88 == apdu->sw2)))
	{
		epass2003_refresh(card);
		r = sc_transmit_apdu(card, apdu);
	}
	return r;
}