static int coolkey_rsa_op(sc_card_t *card,
					const u8 * data, size_t datalen,
					u8 * out, size_t max_out_len)
{
	int r;
	const u8 *crypt_in;
	u8 **crypt_out_p;
	size_t crypt_in_len, *crypt_out_len_p;
	coolkey_private_data_t * priv = COOLKEY_DATA(card);
	coolkey_compute_crypt_params_t params;
	u8 key_number;
	size_t params_len;
	size_t buf_len;
	u8 buf[MAX_COMPUTE_BUF+2];
	u8 *buf_out;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);
	sc_log(card->ctx, 
		 "datalen=%"SC_FORMAT_LEN_SIZE_T"u outlen=%"SC_FORMAT_LEN_SIZE_T"u\n",
		 datalen, max_out_len);

	crypt_in = data;
	crypt_in_len = datalen;

	buf_out = &buf[0];
	crypt_out_p = &buf_out;
	buf_len = sizeof(buf);
	crypt_out_len_p = &buf_len;
	key_number = priv->key_id;
	params.init.mode = COOLKEY_CRYPT_MODE_RSA_NO_PAD;
	params.init.location = COOLKEY_CRYPT_LOCATION_APDU;
	params.init.direction = COOLKEY_CRYPT_DIRECTION_ENCRYPT; /* for no pad, direction is irrelevant */

	if (priv->key_id > 0xff) {
		r = SC_ERROR_NO_DEFAULT_KEY;
		goto done;
	}

	params_len = sizeof(params.init) + crypt_in_len;

	/* send the data to the card if necessary */
	if (crypt_in_len > MAX_COMPUTE_BUF) {
		u8 len_buf[2];
		params.init.location = COOLKEY_CRYPT_LOCATION_DL_OBJECT;
		params_len = sizeof(params.init);
		crypt_in = NULL;
		crypt_in_len = 0;
		*crypt_out_p = NULL;
		*crypt_out_len_p = 0;

		ushort2bebytes(len_buf, datalen);

		r = coolkey_write_object(card, COOLKEY_DL_OBJECT_ID, 0, len_buf, sizeof(len_buf),
					priv->nonce, sizeof(priv->nonce));
		if (r < 0) {
			goto done;
		}

		r = coolkey_write_object(card, COOLKEY_DL_OBJECT_ID, 2, data, datalen, priv->nonce,
						sizeof(priv->nonce));
		if (r < 0) {
			goto done;
		}

	}
	ushort2bebytes(params.init.buf_len, crypt_in_len);
	if (crypt_in_len) {
		memcpy(params.buf, crypt_in, crypt_in_len);
	}


	r = coolkey_apdu_io(card, COOLKEY_CLASS, COOLKEY_INS_COMPUTE_CRYPT,
			key_number, COOLKEY_CRYPT_ONE_STEP, (u8 *)&params, params_len,
			crypt_out_p, crypt_out_len_p, priv->nonce, sizeof(priv->nonce));

	if (r < 0) {
		goto done;
	}
	if (datalen > MAX_COMPUTE_BUF) {
		u8 len_buf[2];
		size_t out_length;

		r = coolkey_read_object(card, COOLKEY_DL_OBJECT_ID, 0, len_buf, sizeof(len_buf),
					priv->nonce, sizeof(priv->nonce));
		if (r < 0) {
			goto done;
		}

		out_length = bebytes2ushort(len_buf);
		out_length = MIN(out_length,max_out_len);

		r = coolkey_read_object(card, COOLKEY_DL_OBJECT_ID, sizeof(len_buf), out, out_length,
					priv->nonce, sizeof(priv->nonce));

	} else {
		size_t out_length = bebytes2ushort(buf);
		out_length = MIN(out_length, max_out_len);
		memcpy(out, buf+2, out_length);
		r = out_length;
	}

done:
	return r;
}