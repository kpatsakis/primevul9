static int coolkey_finish(sc_card_t *card)
{
	coolkey_private_data_t * priv = COOLKEY_DATA(card);

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);
	if (priv) {
		coolkey_free_private_data(priv);
	}
	return SC_SUCCESS;
}