static int coolkey_write_binary(sc_card_t *card, unsigned int idx,
		const u8 *buf, size_t count, unsigned long flags)
{

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);
	LOG_FUNC_RETURN(card->ctx, SC_ERROR_NOT_SUPPORTED);
}