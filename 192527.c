static int coolkey_write_object(sc_card_t *card, unsigned long object_id,
			size_t offset, const u8 *buf, size_t buf_len, const u8 *nonce, size_t nonce_size)
{
	coolkey_write_object_param_t params;
	size_t operation_len;
	size_t left = buf_len;
	int r;
	size_t max_operation_len;

	/* set limit for the card's maximum send size and short write */
	max_operation_len = MIN(COOLKEY_MAX_CHUNK_SIZE, (card->max_send_size - sizeof(coolkey_read_object_param_t) - nonce_size));

	ulong2bebytes(&params.head.object_id[0], object_id);

	do {
		ulong2bebytes(&params.head.offset[0], offset);
		operation_len = MIN(left, max_operation_len);
		params.head.length = operation_len;
		memcpy(params.buf, buf, operation_len);
		r = coolkey_apdu_io(card, COOLKEY_CLASS, COOLKEY_INS_WRITE_OBJECT, 0, 0,
			(u8 *)&params, sizeof(params.head)+operation_len, NULL, 0, nonce, nonce_size);
		if (r < 0) {
			goto fail;
		}
		buf += operation_len;
		offset += operation_len;
		left -= operation_len;
	} while (left != 0);

	return buf_len - left;

fail:
	return r;
}