static int muscle_card_ctl(sc_card_t *card, unsigned long request, void *data)
{
	switch(request) {
	case SC_CARDCTL_MUSCLE_GENERATE_KEY:
		return muscle_card_generate_key(card, (sc_cardctl_muscle_gen_key_info_t*) data);
	case SC_CARDCTL_MUSCLE_EXTRACT_KEY:
		return muscle_card_extract_key(card, (sc_cardctl_muscle_key_info_t*) data);
	case SC_CARDCTL_MUSCLE_IMPORT_KEY:
		return muscle_card_import_key(card, (sc_cardctl_muscle_key_info_t*) data);
	case SC_CARDCTL_MUSCLE_VERIFIED_PINS:
		return muscle_card_verified_pins(card, (sc_cardctl_muscle_verified_pins_info_t*) data);
	default:
		return SC_ERROR_NOT_SUPPORTED; /* Unsupported.. whatever it is */
	}
}