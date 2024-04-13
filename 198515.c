cac_get_acr(sc_card_t *card, int acr_type, u8 **out_buf, size_t *out_len)
{
	u8 *out = NULL;
	/* XXX assuming it will not be longer than 255 B */
	size_t len = 256;
	int r;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);

	/* for simplicity we support only ACR without arguments now */
	if (acr_type != 0x00 && acr_type != 0x10
	    && acr_type != 0x20 && acr_type != 0x21) {
		return SC_ERROR_INVALID_ARGUMENTS;
	}

	r = cac_apdu_io(card, CAC_INS_GET_ACR, acr_type, 0, NULL, 0, &out, &len);
	if (len == 0) {
		r = SC_ERROR_FILE_NOT_FOUND;
	}
	if (r < 0)
		goto fail;

	sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE,
	    "got %"SC_FORMAT_LEN_SIZE_T"u bytes out=%p", len, out);

	*out_len = len;
	*out_buf = out;
	return SC_SUCCESS;

fail:
	if (out)
		free(out);
	*out_buf = NULL;
	*out_len = 0;
	return r;
}