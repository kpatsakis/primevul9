static int esteid_detect_card(sc_pkcs15_card_t *p15card)
{
	if (is_esteid_card(p15card->card))
		return SC_SUCCESS;
	else
		return SC_ERROR_WRONG_CARD;
}