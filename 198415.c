epass2003_select_fid_(struct sc_card *card, sc_path_t * in_path, sc_file_t ** file_out)
{
	struct sc_apdu apdu;
	u8 buf[SC_MAX_APDU_BUFFER_SIZE] = { 0 };
	u8 pathbuf[SC_MAX_PATH_SIZE], *path = pathbuf;
	int r, pathlen;
	sc_file_t *file = NULL;

	epass2003_hook_path(in_path, 1);
	memcpy(path, in_path->value, in_path->len);
	pathlen = in_path->len;

	sc_format_apdu(card, &apdu, SC_APDU_CASE_4_SHORT, 0xA4, 0x00, 0x00);

	switch (in_path->type) {
	case SC_PATH_TYPE_FILE_ID:
		apdu.p1 = 0;
		if (pathlen != 2)
			return SC_ERROR_INVALID_ARGUMENTS;
		break;
	default:
		LOG_FUNC_RETURN(card->ctx, SC_ERROR_INVALID_ARGUMENTS);
	}
	apdu.p2 = 0;		/* first record, return FCI */
	apdu.lc = pathlen;
	apdu.data = path;
	apdu.datalen = pathlen;

	if (file_out != NULL) {
		apdu.resp = buf;
		apdu.resplen = sizeof(buf);
		apdu.le = 0;
	}
	else   {
		apdu.cse = (apdu.lc == 0) ? SC_APDU_CASE_1 : SC_APDU_CASE_3_SHORT;
	}

	if (path[0] == 0x29) {	/* TODO:0x29 accords with FID prefix in profile  */
		/* Not allowed to select private key file, so fake fci. */
		/* 62 16 82 02 11 00 83 02 29 00 85 02 08 00 86 08 FF 90 90 90 FF FF FF FF */
		apdu.resplen = 0x18;
		memcpy(apdu.resp,
		       "\x6f\x16\x82\x02\x11\x00\x83\x02\x29\x00\x85\x02\x08\x00\x86\x08\xff\x90\x90\x90\xff\xff\xff\xff",
		       apdu.resplen);
		apdu.resp[9] = path[1];
		apdu.sw1 = 0x90;
		apdu.sw2 = 0x00;
	}
	else {
		r = sc_transmit_apdu_t(card, &apdu);
		LOG_TEST_RET(card->ctx, r, "APDU transmit failed");
	}

	if (file_out == NULL) {
		if (apdu.sw1 == 0x61)
			LOG_FUNC_RETURN(card->ctx, 0);
		LOG_FUNC_RETURN(card->ctx, sc_check_sw(card, apdu.sw1, apdu.sw2));
	}

	r = sc_check_sw(card, apdu.sw1, apdu.sw2);
	if (r)
		LOG_FUNC_RETURN(card->ctx, r);
	if (apdu.resplen < 2)
		LOG_FUNC_RETURN(card->ctx, SC_ERROR_UNKNOWN_DATA_RECEIVED);

	switch (apdu.resp[0]) {
	case 0x6F:
		file = sc_file_new();
		if (file == NULL)
			LOG_FUNC_RETURN(card->ctx, SC_ERROR_OUT_OF_MEMORY);
		file->path = *in_path;
		if (card->ops->process_fci == NULL) {
			sc_file_free(file);
			LOG_FUNC_RETURN(card->ctx, SC_ERROR_NOT_SUPPORTED);
		}

		if ((size_t) apdu.resp[1] + 2 <= apdu.resplen)
			card->ops->process_fci(card, file, apdu.resp + 2, apdu.resp[1]);
		epass2003_hook_file(file, 0);
		*file_out = file;
		break;
	case 0x00:		/* proprietary coding */
		LOG_FUNC_RETURN(card->ctx, SC_ERROR_UNKNOWN_DATA_RECEIVED);
		break;
	default:
		LOG_FUNC_RETURN(card->ctx, SC_ERROR_UNKNOWN_DATA_RECEIVED);
	}
	return 0;
}