static int gemsafe_detect_card(sc_pkcs15_card_t *p15card)
{
	sc_card_t *card = p15card->card;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);


	if (strcmp(card->name, "Gemplus GPK"))
		return SC_ERROR_WRONG_CARD;
	
	return SC_SUCCESS;
}