static int gemsafe_detect_card( sc_pkcs15_card_t *p15card)
{
	if (strcmp(p15card->card->name, "GemSAFE V1"))
		return SC_ERROR_WRONG_CARD;

	return SC_SUCCESS;
}