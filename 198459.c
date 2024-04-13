static int muscle_compute_signature(sc_card_t *card, const u8 *data,
		size_t data_len, u8 * out, size_t outlen)
{
	muscle_private_t* priv = MUSCLE_DATA(card);
	u8 key_id;
	int r;

	key_id = priv->rsa_key_ref * 2; /* Private key */

	if (outlen < data_len) {
		sc_debug(card->ctx, SC_LOG_DEBUG_NORMAL, "Output buffer too small");
		return SC_ERROR_BUFFER_TOO_SMALL;
	}

	r = msc_compute_crypt(card,
		key_id,
		0x00, /* RSA NO PADDING */
		0x04, /* -- decrypt raw... will do what we need since signing isn't yet supported */
		data,
		out,
		data_len,
		outlen);
	SC_TEST_RET(card->ctx, SC_LOG_DEBUG_NORMAL, r, "Card signature failed");
	return r;
}