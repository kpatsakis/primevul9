gen_init_key(struct sc_card *card, unsigned char *key_enc, unsigned char *key_mac,
		unsigned char *result, unsigned char key_type)
{
	int r;
	struct sc_apdu apdu;
	unsigned char data[256] = { 0 };
	unsigned char tmp_sm;
	unsigned long blocksize = 0;
	unsigned char cryptogram[256] = { 0 };	/* host cryptogram */
	unsigned char iv[16] = { 0 };
	epass2003_exdata *exdata = NULL;
	
	if (!card->drv_data) 
		return SC_ERROR_INVALID_ARGUMENTS;
	
	exdata = (epass2003_exdata *)card->drv_data;

	LOG_FUNC_CALLED(card->ctx);

	sc_format_apdu(card, &apdu, SC_APDU_CASE_4_SHORT, 0x50, 0x00, 0x00);
	apdu.cla = 0x80;
	apdu.lc = apdu.datalen = sizeof(g_random);
	apdu.data = g_random;	/* host random */
	apdu.le = apdu.resplen = 28;
	apdu.resp = result;	/* card random is result[12~19] */

	tmp_sm = exdata->sm;
	exdata->sm = SM_PLAIN;
	r = epass2003_transmit_apdu(card, &apdu);
	exdata->sm = tmp_sm;
	LOG_TEST_RET(card->ctx, r, "APDU gen_init_key failed");

	r = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(card->ctx, r, "gen_init_key failed");

	/* Step 1 - Generate Derivation data */
	memcpy(data, &result[16], 4);
	memcpy(&data[4], g_random, 4);
	memcpy(&data[8], &result[12], 4);
	memcpy(&data[12], &g_random[4], 4);

	/* Step 2,3 - Create S-ENC/S-MAC Session Key */
	if (KEY_TYPE_AES == key_type) {
		aes128_encrypt_ecb(key_enc, 16, data, 16, exdata->sk_enc);
		aes128_encrypt_ecb(key_mac, 16, data, 16, exdata->sk_mac);
	}
	else {
		des3_encrypt_ecb(key_enc, 16, data, 16, exdata->sk_enc);
		des3_encrypt_ecb(key_mac, 16, data, 16, exdata->sk_mac);
	}

	memcpy(data, g_random, 8);
	memcpy(&data[8], &result[12], 8);
	data[16] = 0x80;
	blocksize = (key_type == KEY_TYPE_AES ? 16 : 8);
	memset(&data[17], 0x00, blocksize - 1);

	/* calculate host cryptogram */
	if (KEY_TYPE_AES == key_type)
		aes128_encrypt_cbc(exdata->sk_enc, 16, iv, data, 16 + blocksize, cryptogram);
	else
		des3_encrypt_cbc(exdata->sk_enc, 16, iv, data, 16 + blocksize, cryptogram);

	/* verify card cryptogram */
	if (0 != memcmp(&cryptogram[16], &result[20], 8))
		LOG_FUNC_RETURN(card->ctx, SC_ERROR_CARD_CMD_FAILED);

	LOG_FUNC_RETURN(card->ctx, SC_SUCCESS);
}