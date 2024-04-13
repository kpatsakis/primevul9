update_secret_key(struct sc_card *card, unsigned char ktype, unsigned char kid,
		const unsigned char *data, unsigned long datalen)
{
	int r;
	struct sc_apdu apdu;
	unsigned char hash[HASH_LEN] = { 0 };
	unsigned char tmp_data[256] = { 0 };
	unsigned char maxtries = 0;

	r = hash_data(data, datalen, hash, SC_ALGORITHM_ECDSA_HASH_SHA1);
	LOG_TEST_RET(card->ctx, r, "hash data failed");

	r = get_external_key_maxtries(card, &maxtries);
	LOG_TEST_RET(card->ctx, r, "get max counter failed");

	tmp_data[0] = (maxtries << 4) | maxtries;
	memcpy(&tmp_data[1], hash, HASH_LEN);
	sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0xe5, ktype, kid);
	apdu.cla = 0x80;
	apdu.lc = apdu.datalen = 1 + HASH_LEN;
	apdu.data = tmp_data;

	r = sc_transmit_apdu_t(card, &apdu);
	LOG_TEST_RET(card->ctx, r, "APDU update_secret_key failed");
	r = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(card->ctx, r, "update_secret_key failed");

	return r;
}