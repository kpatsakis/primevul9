static int muscle_card_reader_lock_obtained(sc_card_t *card, int was_reset)
{
	int r = SC_SUCCESS;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);

	if (was_reset > 0) {
		if (msc_select_applet(card, muscleAppletId, sizeof muscleAppletId) != 1) {
			r = SC_ERROR_INVALID_CARD;
		}
	}

	LOG_FUNC_RETURN(card->ctx, r);
}