coolkey_process_combined_object(sc_card_t *card, coolkey_private_data_t *priv, u8 *object, size_t object_length)
{
	coolkey_combined_header_t *header = (coolkey_combined_header_t *)object;
	unsigned short compressed_offset;
	unsigned short compressed_length;
	unsigned short compressed_type;
	unsigned short object_offset;
	unsigned short object_count;
	coolkey_decompressed_header_t *decompressed_header;
	u8 *decompressed_object = NULL;
	size_t decompressed_object_len = 0;
	int free_decompressed = 0;
	int i, r;

	if (object_length < sizeof(coolkey_combined_header_t)) {
		return SC_ERROR_CORRUPTED_DATA;
	}
	compressed_offset = bebytes2ushort(header->compression_offset);
	compressed_length = bebytes2ushort(header->compression_length);
	compressed_type   = bebytes2ushort(header->compression_type);

	if ((((size_t)compressed_offset) + (size_t)compressed_length) >  object_length) {
		return SC_ERROR_CORRUPTED_DATA;
	}

	/* store the CUID */
	memcpy(&priv->cuid, &header->cuid, sizeof(priv->cuid));

	if (compressed_type == COOLKEY_COMPRESSION_ZLIB) {
#ifdef ENABLE_ZLIB
		r = sc_decompress_alloc(&decompressed_object, &decompressed_object_len, &object[compressed_offset], compressed_length, COMPRESSION_AUTO);
		if (r)
			goto done;
		free_decompressed = 1;
#else
		sc_log(card->ctx, "Coolkey compression not supported, no zlib");
		LOG_FUNC_RETURN(card->ctx, SC_ERROR_NOT_SUPPORTED);
#endif
	}  else {
		decompressed_object =&object[compressed_offset];
		decompressed_object_len = (size_t) compressed_length;
	}

	decompressed_header = (coolkey_decompressed_header_t *)decompressed_object;

	if (decompressed_object_len < sizeof(coolkey_decompressed_header_t)) {
		return SC_ERROR_CORRUPTED_DATA;
	}
	object_offset = bebytes2ushort(decompressed_header->object_offset);
	object_count = bebytes2ushort(decompressed_header->object_count);


	/*
	 * using 2 different tests here so we can log different errors if logging is
	 * turned on.
	 */
	/* make sure token_name doesn't overrun the buffer */
	if (decompressed_header->token_name_length +
		offsetof(coolkey_decompressed_header_t,token_name) > decompressed_object_len) {
		r = SC_ERROR_CORRUPTED_DATA;
		goto done;
	}
	/* make sure it doesn't overlap the object space */
	if (decompressed_header->token_name_length +
		offsetof(coolkey_decompressed_header_t,token_name) > object_offset) {
		r = SC_ERROR_CORRUPTED_DATA;
		goto done;
	}

	/* store the token name in the priv structure so the emulator can set it */
	priv->token_name = malloc(decompressed_header->token_name_length+1);
	if (priv->token_name == NULL) {
		r = SC_ERROR_OUT_OF_MEMORY;
		goto done;
	}
	memcpy(priv->token_name, &decompressed_header->token_name[0],
							decompressed_header->token_name_length);
	priv->token_name[decompressed_header->token_name_length] = 0;
	priv->token_name_length = decompressed_header->token_name_length;


	for (i=0; i < object_count && object_offset < decompressed_object_len; i++ ) {
		u8 *current_object = &decompressed_object[object_offset];
		coolkey_combined_object_header_t *object_header =
				(coolkey_combined_object_header_t *)current_object;
		unsigned long object_id = bebytes2ulong(object_header->object_id);
		int current_object_len;

		/* figure out how big it is */
		r = coolkey_v1_get_object_length(current_object, decompressed_object_len-object_offset);
		if (r < 0) {
			goto done;
		}
		if ((size_t)r + object_offset > decompressed_object_len) {
			r = SC_ERROR_CORRUPTED_DATA;
			goto done;
		}
		current_object_len = r;
		object_offset += current_object_len;

		/* record this object */
		r = coolkey_add_object(priv, object_id, current_object, current_object_len, 1);
		if (r) {
			goto done;
		}

	}
	r = SC_SUCCESS;

done:
	if (free_decompressed) {
		free(decompressed_object);
	}
	return r;
}