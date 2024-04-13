external_key_auth(struct sc_card *card, unsigned char kid,
		unsigned char *data, size_t datalen)
{
	int r;
	struct sc_apdu apdu;
	unsigned char random[16] = { 0 };
	unsigned char tmp_data[16] = { 0 };
	unsigned char hash[HASH_LEN] = { 0 };
	unsigned char iv[16] = { 0 };

	r = sc_get_challenge(card, random, 8);
	LOG_TEST_RET(card->ctx, r, "get challenge external_key_auth failed");

	r = hash_data(data, datalen, hash, SC_ALGORITHM_ECDSA_HASH_SHA1);
	LOG_TEST_RET(card->ctx, r, "hash data failed");

	des3_encrypt_cbc(hash, HASH_LEN, iv, random, 8, tmp_data);
	sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0x82, 0x01, 0x80 | kid);
	apdu.lc = apdu.datalen = 8;
	apdu.data = tmp_data;

	r = sc_transmit_apdu_t(card, &apdu);
	LOG_TEST_RET(card->ctx, r, "APDU external_key_auth failed");
	r = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(card->ctx, r, "external_key_auth failed");

	return r;
}