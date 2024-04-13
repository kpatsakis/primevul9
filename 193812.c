static int setcos_card_ctl(sc_card_t *card, unsigned long cmd, void *ptr)
{
	if (card->type != SC_CARD_TYPE_SETCOS_44 && !SETCOS_IS_EID_APPLET(card))
		return SC_ERROR_NOT_SUPPORTED;

	switch(cmd) {
	case SC_CARDCTL_SETCOS_PUTDATA:
		return setcos_putdata(card,
				(struct sc_cardctl_setcos_data_obj*) ptr);
		break;
	case SC_CARDCTL_SETCOS_GETDATA:
		return setcos_getdata(card,
				(struct sc_cardctl_setcos_data_obj*) ptr);
		break;
	case SC_CARDCTL_SETCOS_GENERATE_STORE_KEY:
		return setcos_generate_store_key(card,
				(struct sc_cardctl_setcos_gen_store_key_info *) ptr);
	case SC_CARDCTL_SETCOS_ACTIVATE_FILE:
		return setcos_activate_file(card);
	}

	return SC_ERROR_NOT_SUPPORTED;
}