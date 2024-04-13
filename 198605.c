epass2003_delete_file(struct sc_card *card, const sc_path_t * path)
{
	int r;
	u8 sbuf[2];
	struct sc_apdu apdu;

	LOG_FUNC_CALLED(card->ctx);

	r = sc_select_file(card, path, NULL);
	epass2003_hook_path((struct sc_path *)path, 1);
	if (r == SC_SUCCESS) {
		sbuf[0] = path->value[path->len - 2];
		sbuf[1] = path->value[path->len - 1];
		sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0xE4, 0x00, 0x00);
		apdu.lc = 2;
		apdu.datalen = 2;
		apdu.data = sbuf;
	}
	else   {
		LOG_FUNC_RETURN(card->ctx, SC_ERROR_INVALID_ARGUMENTS);
	}

	r = sc_transmit_apdu_t(card, &apdu);
	LOG_TEST_RET(card->ctx, r, "APDU transmit failed");
	r = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(card->ctx, r, "Delete file failed");

	LOG_FUNC_RETURN(card->ctx, r);
}