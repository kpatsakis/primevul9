static int coolkey_read_object(sc_card_t *card, unsigned long object_id, size_t offset,
			u8 *out_buf, size_t out_len, u8 *nonce, size_t nonce_size)
{
	coolkey_read_object_param_t params;
	u8 *out_ptr;
	size_t left = 0;
	size_t len;
	int r;

	ulong2bebytes(&params.object_id[0], object_id);

	out_ptr = out_buf;
	left = out_len;
	do {
		ulong2bebytes(&params.offset[0], offset);
		params.length = MIN(left, COOLKEY_MAX_CHUNK_SIZE);
		len = left;
		r = coolkey_apdu_io(card, COOLKEY_CLASS, COOLKEY_INS_READ_OBJECT, 0, 0,
			(u8 *)&params, sizeof(params), &out_ptr, &len, nonce, nonce_size);
		if (r < 0) {
			goto fail;
		}
		/* sanity check to make sure we don't overflow left */
		if ((left < len) || (len == 0)) {
			r = SC_ERROR_INTERNAL;
			goto fail;
		}
		out_ptr += len;
		offset += len;
		left -= len;
	} while (left != 0);

	return out_len;

fail:
	return r;
}