select_esteid_df (sc_card_t * card)
{
	int r;
	sc_path_t tmppath;
	sc_format_path ("3F00EEEE", &tmppath);
	r = sc_select_file (card, &tmppath, NULL);
	SC_TEST_RET(card->ctx, SC_LOG_DEBUG_NORMAL, r, "esteid select DF failed");
	return r;
}