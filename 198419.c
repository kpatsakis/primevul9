static int epass2003_match_card(struct sc_card *card)
{
	int r;

	LOG_FUNC_CALLED(card->ctx);
	r = _sc_match_atr(card, epass2003_atrs, &card->type);
	if (r < 0)
		return 0;

	return 1;
}