static int tcos_delete_file(sc_card_t *card, const sc_path_t *path)
{
	int r;
	u8 sbuf[2];
	sc_apdu_t apdu;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);
	if (path->type != SC_PATH_TYPE_FILE_ID && path->len != 2) {
		sc_debug(card->ctx, SC_LOG_DEBUG_NORMAL, "File type has to be SC_PATH_TYPE_FILE_ID\n");
		SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_NORMAL, SC_ERROR_INVALID_ARGUMENTS);
	}
	sbuf[0] = path->value[0];
	sbuf[1] = path->value[1];
	sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0xE4, 0x00, 0x00);
        apdu.cla |= 0x80;
	apdu.lc = 2;
	apdu.datalen = 2;
	apdu.data = sbuf;
	
	r = sc_transmit_apdu(card, &apdu);
	SC_TEST_RET(card->ctx, SC_LOG_DEBUG_NORMAL, r, "APDU transmit failed");
	return sc_check_sw(card, apdu.sw1, apdu.sw2);
}