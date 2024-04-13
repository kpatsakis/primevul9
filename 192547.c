static int coolkey_select_applet(sc_card_t *card)
{
	u8 aid[] = { 0x62, 0x76, 0x01, 0xff, 0x00, 0x00, 0x00 };
	return coolkey_apdu_io(card, ISO7816_CLASS, ISO7816_INS_SELECT_FILE, 4, 0,
			&aid[0], sizeof(aid), NULL, NULL,  NULL, 0);
}