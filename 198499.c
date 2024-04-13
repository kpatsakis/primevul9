verify_init_key(struct sc_card *card, unsigned char *ran_key, unsigned char key_type)
{
	int r;
	struct sc_apdu apdu;
	unsigned long blocksize = (key_type == KEY_TYPE_AES ? 16 : 8);
	unsigned char data[256] = { 0 };
	unsigned char cryptogram[256] = { 0 };	/* host cryptogram */
	unsigned char iv[16] = { 0 };
	unsigned char mac[256] = { 0 };
	unsigned long i;
	unsigned char tmp_sm;
	epass2003_exdata *exdata = NULL;

	if (!card->drv_data) 
		return SC_ERROR_INVALID_ARGUMENTS;
	exdata = (epass2003_exdata *)card->drv_data;

	LOG_FUNC_CALLED(card->ctx);

	memcpy(data, ran_key, 8);
	memcpy(&data[8], g_random, 8);
	data[16] = 0x80;
	memset(&data[17], 0x00, blocksize - 1);
	memset(iv, 0, 16);

	/* calculate host cryptogram */
	if (KEY_TYPE_AES == key_type) {
		aes128_encrypt_cbc(exdata->sk_enc, 16, iv, data, 16 + blocksize,
				   cryptogram);
	} else {
		des3_encrypt_cbc(exdata->sk_enc, 16, iv, data, 16 + blocksize,
				 cryptogram);
	}

	memset(data, 0, sizeof(data));
	memcpy(data, "\x84\x82\x03\x00\x10", 5);
	memcpy(&data[5], &cryptogram[16], 8);
	memcpy(&data[13], "\x80\x00\x00", 3);

	/* calculate mac icv */
	memset(iv, 0x00, 16);
	if (KEY_TYPE_AES == key_type) {
		aes128_encrypt_cbc(exdata->sk_mac, 16, iv, data, 16, mac);
		i = 0;
	} else {
		des3_encrypt_cbc(exdata->sk_mac, 16, iv, data, 16, mac);
		i = 8;
	}
	/* save mac icv */
	memset(exdata->icv_mac, 0x00, 16);
	memcpy(exdata->icv_mac, &mac[i], 8);

	/* verify host cryptogram */
	memcpy(data, &cryptogram[16], 8);
	memcpy(&data[8], &mac[i], 8);
	sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0x82, 0x03, 0x00);
	apdu.cla = 0x84;
	apdu.lc = apdu.datalen = 16;
	apdu.data = data;
	tmp_sm = exdata->sm;
	exdata->sm = SM_PLAIN;
	r = epass2003_transmit_apdu(card, &apdu);
	exdata->sm = tmp_sm;
	LOG_TEST_RET(card->ctx, r,
		    "APDU verify_init_key failed");
	r = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(card->ctx, r,
		    "verify_init_key failed");
	return r;
}