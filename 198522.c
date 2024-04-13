epass2003_card_ctl(struct sc_card *card, unsigned long cmd, void *ptr)
{
	LOG_FUNC_CALLED(card->ctx);

	sc_log(card->ctx, "cmd is %0lx", cmd);
	switch (cmd) {
	case SC_CARDCTL_ENTERSAFE_WRITE_KEY:
		return epass2003_write_key(card, (sc_epass2003_wkey_data *) ptr);
	case SC_CARDCTL_ENTERSAFE_GENERATE_KEY:
		return epass2003_gen_key(card, (sc_epass2003_gen_key_data *) ptr);
	case SC_CARDCTL_ERASE_CARD:
		return epass2003_erase_card(card);
	case SC_CARDCTL_GET_SERIALNR:
		return epass2003_get_serialnr(card, (sc_serial_number_t *) ptr);
	default:
		return SC_ERROR_NOT_SUPPORTED;
	}
}