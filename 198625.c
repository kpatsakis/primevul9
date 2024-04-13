static int cac_get_challenge(sc_card_t *card, u8 *rnd, size_t len)
{
	/* CAC requires 8 byte response */
	u8 rbuf[8];
	u8 *rbufp = &rbuf[0];
	size_t out_len = sizeof rbuf;
	int r;

	LOG_FUNC_CALLED(card->ctx);

	r = cac_apdu_io(card, 0x84, 0x00, 0x00, NULL, 0, &rbufp, &out_len);
	LOG_TEST_RET(card->ctx, r, "Could not get challenge");

	if (len < out_len) {
		out_len = len;
	}
	memcpy(rnd, rbuf, out_len);

	SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_NORMAL, (int) out_len);
}