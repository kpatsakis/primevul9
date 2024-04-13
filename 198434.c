static int cac_read_file(sc_card_t *card, int file_type, u8 **out_buf, size_t *out_len)
{
	u8 params[2];
	u8 count[2];
	u8 *out = NULL;
	u8 *out_ptr;
	size_t offset = 0;
	size_t size = 0;
	size_t left = 0;
	size_t len;
	int r;

	params[0] = file_type;
	params[1] = 2;

	/* get the size */
	len = sizeof(count);
	out_ptr = count;
	r = cac_apdu_io(card, CAC_INS_READ_FILE, 0, 0, &params[0], sizeof(params), &out_ptr, &len);
	if (len == 0) {
		r = SC_ERROR_FILE_NOT_FOUND;
	}
	if (r < 0)
		goto fail;

	left = size = lebytes2ushort(count);
	sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE,
		 "got %"SC_FORMAT_LEN_SIZE_T"u bytes out_ptr=%p count&=%p count[0]=0x%02x count[1]=0x%02x, len=0x%04"SC_FORMAT_LEN_SIZE_T"x (%"SC_FORMAT_LEN_SIZE_T"u)",
		 len, out_ptr, &count, count[0], count[1], size, size);
	out = out_ptr = malloc(size);
	if (out == NULL) {
		r = SC_ERROR_OUT_OF_MEMORY;
		goto fail;
	}
	for (offset += 2; left > 0; offset += len, left -= len, out_ptr += len) {
		len = MIN(left, CAC_MAX_CHUNK_SIZE);
		params[1] = len;
		r = cac_apdu_io(card, CAC_INS_READ_FILE, HIGH_BYTE_OF_SHORT(offset), LOW_BYTE_OF_SHORT(offset),
						&params[0], sizeof(params), &out_ptr, &len);
		/* if there is no data, assume there is no file */
		if (len == 0) {
			r = SC_ERROR_FILE_NOT_FOUND;
		}
		if (r < 0) {
			goto fail;
		}
	}
	*out_len = size;
	*out_buf = out;
	return SC_SUCCESS;
fail:
	if (out)
		free(out);
	*out_len = 0;
	return r;
}