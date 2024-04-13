compute_binder_key(const mac_entry_st *prf,
		   const uint8_t *key, size_t keylen,
		   bool resuming,
		   void *out)
{
	int ret;
	const char ext_label[] = EXT_BINDER_LABEL;
	const size_t ext_label_len = sizeof(ext_label) - 1;
	const char res_label[] = RES_BINDER_LABEL;
	const size_t res_label_len = sizeof(res_label) - 1;
	const char *label = resuming ? res_label : ext_label;
	size_t label_len = resuming ? res_label_len : ext_label_len;
	uint8_t tmp_key[MAX_HASH_SIZE];

	/* Compute HKDF-Extract(0, psk) */
	ret = _tls13_init_secret2(prf, key, keylen, tmp_key);
	if (ret < 0)
		return ret;

	/* Compute Derive-Secret(secret, label, transcript_hash) */
	ret = _tls13_derive_secret2(prf, label, label_len,
				    NULL, 0, tmp_key, out);
	if (ret < 0)
		return ret;

	return 0;
}