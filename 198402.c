epass2003_sm_free_wrapped_apdu(struct sc_card *card,
		struct sc_apdu *plain, struct sc_apdu **sm_apdu)
{
	struct sc_context *ctx = card->ctx;
	int rv = SC_SUCCESS;

	LOG_FUNC_CALLED(ctx);
	if (!sm_apdu)
		LOG_FUNC_RETURN(ctx, SC_ERROR_INVALID_ARGUMENTS);
	if (!(*sm_apdu))
		LOG_FUNC_RETURN(ctx, SC_SUCCESS);


	if (plain)
		rv = epass2003_sm_unwrap_apdu(card, *sm_apdu, plain);

	if ((*sm_apdu)->data) {
		unsigned char * p = (unsigned char *)((*sm_apdu)->data);
		free(p);
	}
	if ((*sm_apdu)->resp) {
		free((*sm_apdu)->resp);
	}

	free(*sm_apdu);
	*sm_apdu = NULL;

	LOG_FUNC_RETURN(ctx, rv);
}