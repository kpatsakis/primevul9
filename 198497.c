util_connect_card(sc_context_t *ctx, sc_card_t **cardp,
		 const char *reader_id, int do_wait, int verbose)
{
	return util_connect_card_ex(ctx, cardp, reader_id, do_wait, 1, verbose);
}