static int cac_init(sc_card_t *card)
{
	int r;
	unsigned long flags;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);

	r = cac_find_and_initialize(card, 1);
	if (r < 0) {
		SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_NORMAL, SC_ERROR_INVALID_CARD);
	}
	flags = SC_ALGORITHM_RSA_RAW;

	_sc_card_add_rsa_alg(card, 1024, flags, 0); /* mandatory */
	_sc_card_add_rsa_alg(card, 2048, flags, 0); /* optional */
	_sc_card_add_rsa_alg(card, 3072, flags, 0); /* optional */

	card->caps |= SC_CARD_CAP_RNG | SC_CARD_CAP_ISO7816_PIN_INFO;

	SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_NORMAL, SC_SUCCESS);
}