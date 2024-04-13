static int coolkey_check_sw(sc_card_t *card, unsigned int sw1, unsigned int sw2)
{
	sc_log(card->ctx, 
		"sw1 = 0x%02x, sw2 = 0x%02x\n", sw1, sw2);

	if (sw1 == 0x90 && sw2 == 0x00)
		return SC_SUCCESS;

	if (sw1 == 0x9c) {
		if (sw2 == 0xff) {
			/* shouldn't happen on a production applet, 0x9cff is a debugging error code */
			return SC_ERROR_INTERNAL;
		}
		if (sw2 >= coolkey_number_of_error_codes) {
			return SC_ERROR_UNKNOWN;
		}
		return coolkey_error_codes[sw2].sc_error;
	}

	/* iso error */
        return sc_get_iso7816_driver()->ops->check_sw(card, sw1, sw2);
}