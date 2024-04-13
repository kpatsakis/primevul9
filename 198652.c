static int tcos_list_files(sc_card_t *card, u8 *buf, size_t buflen)
{
	sc_context_t *ctx;
	sc_apdu_t apdu;
	u8 rbuf[SC_MAX_APDU_BUFFER_SIZE], p1;
	int r, count = 0;

	assert(card != NULL);
	ctx = card->ctx;

	for (p1=1; p1<=2; p1++) {
		sc_format_apdu(card, &apdu, SC_APDU_CASE_2_SHORT, 0xAA, p1, 0);
		apdu.cla = 0x80;
		apdu.resp = rbuf;
		apdu.resplen = sizeof(rbuf);
		apdu.le = 256;
		r = sc_transmit_apdu(card, &apdu);
		SC_TEST_RET(ctx, SC_LOG_DEBUG_NORMAL, r, "APDU transmit failed");
		if (apdu.sw1==0x6A && (apdu.sw2==0x82 || apdu.sw2==0x88)) continue;
		r = sc_check_sw(card, apdu.sw1, apdu.sw2);
		SC_TEST_RET(ctx, SC_LOG_DEBUG_NORMAL, r, "List Dir failed");
		if (apdu.resplen > buflen) return SC_ERROR_BUFFER_TOO_SMALL;
		sc_debug(ctx, SC_LOG_DEBUG_NORMAL,
			 "got %"SC_FORMAT_LEN_SIZE_T"u %s-FileIDs\n",
			 apdu.resplen / 2, p1 == 1 ? "DF" : "EF");

		memcpy(buf, apdu.resp, apdu.resplen);
		buf += apdu.resplen;
		buflen -= apdu.resplen;
		count += apdu.resplen;
	}
	return count;
}