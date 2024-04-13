static int coolkey_ecc_op(sc_card_t *card,
					const u8 * data, size_t datalen,
					u8 * out, size_t outlen)
{
	int r;
	const u8 *crypt_in;
	u8  **crypt_out_p;
	u8  ins = 0;
	size_t crypt_in_len, *crypt_out_len_p;
	coolkey_private_data_t * priv = COOLKEY_DATA(card);
	coolkey_compute_ecc_params_t params;
	size_t params_len;
	u8 key_number;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);
	sc_log(card->ctx, 
		 "datalen=%"SC_FORMAT_LEN_SIZE_T"u outlen=%"SC_FORMAT_LEN_SIZE_T"u\n",
		 datalen, outlen);

	crypt_in = data;
	crypt_in_len = datalen;

	crypt_out_p = &out;
	crypt_out_len_p = &outlen;
	key_number = priv->key_id;
	params.location = COOLKEY_CRYPT_LOCATION_APDU;

	if (priv->key_id > 0xff) {
		r = SC_ERROR_NO_DEFAULT_KEY;
		goto done;
	}

	switch (priv->operation) {
	case SC_SEC_OPERATION_DERIVE:
		ins = COOLKEY_INS_COMPUTE_ECC_KEY_AGREEMENT;
		break;
	case SC_SEC_OPERATION_SIGN:
		ins = COOLKEY_INS_COMPUTE_ECC_SIGNATURE;
		break;
	default:
		r = SC_ERROR_NOT_SUPPORTED;
		goto done;
	}

	params_len = (sizeof(params) - sizeof(params.buf))  + crypt_in_len;

	ushort2bebytes(params.buf_len, crypt_in_len);
	if (crypt_in_len) {
		memcpy(params.buf, crypt_in, crypt_in_len);
	}


	r = coolkey_apdu_io(card, COOLKEY_CLASS, ins,
			key_number, COOLKEY_CRYPT_ONE_STEP, (u8 *)&params, params_len,
			crypt_out_p, crypt_out_len_p, priv->nonce, sizeof(priv->nonce));

done:
	return r;
}