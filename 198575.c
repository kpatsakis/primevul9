epass2003_erase_card(struct sc_card *card)
{
	int r;

	LOG_FUNC_CALLED(card->ctx);
	sc_invalidate_cache(card);

	r = sc_delete_file(card, sc_get_mf_path());
	LOG_TEST_RET(card->ctx, r, "delete MF failed");

	LOG_FUNC_RETURN(card->ctx, r);
}