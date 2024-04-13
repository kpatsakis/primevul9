epass2003_sm_get_wrapped_apdu(struct sc_card *card,
		struct sc_apdu *plain, struct sc_apdu **sm_apdu)
{
	struct sc_context *ctx = card->ctx;
	struct sc_apdu *apdu = NULL;
	int rv;

	LOG_FUNC_CALLED(ctx);
	if (!plain || !sm_apdu)
		LOG_FUNC_RETURN(ctx, SC_ERROR_INVALID_ARGUMENTS);

	*sm_apdu = NULL;
	//construct new SM apdu from original apdu
	apdu = calloc(1, sizeof(struct sc_apdu));
	if (!apdu) {
		rv = SC_ERROR_OUT_OF_MEMORY;
		goto err;
	}
	apdu->data = calloc (1, SC_MAX_EXT_APDU_BUFFER_SIZE);
	if (!apdu->data) {
		rv = SC_ERROR_OUT_OF_MEMORY;
		goto err;
	}
	apdu->resp = calloc (1, SC_MAX_EXT_APDU_BUFFER_SIZE);
	if (!apdu->resp) {
		rv = SC_ERROR_OUT_OF_MEMORY;
		goto err;
	}
	apdu->datalen = SC_MAX_EXT_APDU_BUFFER_SIZE;
	apdu->resplen = SC_MAX_EXT_APDU_BUFFER_SIZE;

	rv = epass2003_sm_wrap_apdu(card, plain, apdu);
	if (rv)   {
		rv = epass2003_sm_free_wrapped_apdu(card, NULL, &apdu);
		if (rv < 0)
			goto err;
	}

	*sm_apdu = apdu;
	apdu = NULL;
err:
	if (apdu) {
		free((unsigned char *) apdu->data);
		free(apdu->resp);
		free(apdu);
		apdu = NULL;
	}
	LOG_FUNC_RETURN(ctx, rv);
}