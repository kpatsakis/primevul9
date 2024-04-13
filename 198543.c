static int muscle_get_challenge(sc_card_t *card, u8 *rnd, size_t len)
{
	if (len == 0)
		return SC_SUCCESS;
	else {
		SC_TEST_RET(card->ctx, SC_LOG_DEBUG_NORMAL,
				msc_get_challenge(card, len, 0, NULL, rnd),
				"GET CHALLENGE cmd failed");
		return (int) len;
	}
}