static int coolkey_card_reader_lock_obtained(sc_card_t *card, int was_reset)
{
	int r = SC_SUCCESS;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);

	if (was_reset > 0) {
		r = coolkey_select_applet(card);
	}

	LOG_FUNC_RETURN(card->ctx, r);
}