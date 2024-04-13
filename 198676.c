static int muscle_decipher(sc_card_t * card,
			 const u8 * crgram, size_t crgram_len, u8 * out,
			 size_t out_len)
{
	muscle_private_t* priv = MUSCLE_DATA(card);

	u8 key_id;
	int r;

	/* sanity check */
	if (priv->env.operation != SC_SEC_OPERATION_DECIPHER)
		return SC_ERROR_INVALID_ARGUMENTS;

	key_id = priv->rsa_key_ref * 2; /* Private key */

	if (out_len < crgram_len) {
		sc_debug(card->ctx, SC_LOG_DEBUG_NORMAL, "Output buffer too small");
		return SC_ERROR_BUFFER_TOO_SMALL;
	}

	r = msc_compute_crypt(card,
		key_id,
		0x00, /* RSA NO PADDING */
		0x04, /* decrypt */
		crgram,
		out,
		crgram_len,
		out_len);
	SC_TEST_RET(card->ctx, SC_LOG_DEBUG_NORMAL, r, "Card signature failed");
	return r;
}