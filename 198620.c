epass2003_check_sw(struct sc_card *card, unsigned int sw1, unsigned int sw2)
{
	const int err_count = sizeof(epass2003_errors)/sizeof(epass2003_errors[0]);
	int i;

	/* Handle special cases here */
	if (sw1 == 0x6C) {
		sc_log(card->ctx, "Wrong length; correct length is %d", sw2);
		return SC_ERROR_WRONG_LENGTH;
	}


	for (i = 0; i < err_count; i++)   {
		if (epass2003_errors[i].SWs == ((sw1 << 8) | sw2)) {
			sc_log(card->ctx, "%s", epass2003_errors[i].errorstr);
			return epass2003_errors[i].errorno;
		}
	}

	sc_log(card->ctx, "Unknown SWs; SW1=%02X, SW2=%02X", sw1, sw2);
	return SC_ERROR_CARD_CMD_FAILED;
}