internal_install_pin(struct sc_card *card, sc_epass2003_wkey_data * pin)
{
	int r;
	unsigned char hash[HASH_LEN] = { 0 };

	r = hash_data(pin->key_data.es_secret.key_val, pin->key_data.es_secret.key_len, hash, SC_ALGORITHM_ECDSA_HASH_SHA1);
	LOG_TEST_RET(card->ctx, r, "hash data failed");

	r = install_secret_key(card, 0x04, pin->key_data.es_secret.kid,
			       pin->key_data.es_secret.ac[0],
			       pin->key_data.es_secret.ac[1],
			       pin->key_data.es_secret.EC, hash, HASH_LEN);
	LOG_TEST_RET(card->ctx, r, "Install failed");

	return r;
}