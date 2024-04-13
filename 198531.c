epass2003_gen_key(struct sc_card *card, sc_epass2003_gen_key_data * data)
{
	int r;
	size_t len = data->key_length;
	struct sc_apdu apdu;
	u8 rbuf[SC_MAX_EXT_APDU_BUFFER_SIZE] = { 0 };
	u8 sbuf[SC_MAX_EXT_APDU_BUFFER_SIZE] = { 0 };

	LOG_FUNC_CALLED(card->ctx);

	if(len == 256)
	{
		sbuf[0] = 0x02;
	}
	else
	{
		sbuf[0] = 0x01;
	}
	sbuf[1] = (u8) ((len >> 8) & 0xff);
	sbuf[2] = (u8) (len & 0xff);
	sbuf[3] = (u8) ((data->prkey_id >> 8) & 0xFF);
	sbuf[4] = (u8) ((data->prkey_id) & 0xFF);
	sbuf[5] = (u8) ((data->pukey_id >> 8) & 0xFF);
	sbuf[6] = (u8) ((data->pukey_id) & 0xFF);

	/* generate key */
	sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0x46, 0x00, 0x00);
	apdu.lc = apdu.datalen = 7;
	apdu.data = sbuf;

	r = sc_transmit_apdu_t(card, &apdu);
	LOG_TEST_RET(card->ctx, r, "APDU transmit failed");
	r = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(card->ctx, r, "generate keypair failed");

	/* read public key */
	sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0xb4, 0x02, 0x00);
	if(len == 256)
	{
		apdu.p1 = 0x00;
	}
	apdu.cla = 0x80;
	apdu.lc = apdu.datalen = 2;
	apdu.data = &sbuf[5];
	apdu.resp = rbuf;
	apdu.resplen = sizeof(rbuf);
	apdu.le = 0x00;

	r = sc_transmit_apdu_t(card, &apdu);
	LOG_TEST_RET(card->ctx, r, "APDU transmit failed");
	r = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(card->ctx, r, "get pukey failed");

	if (len < apdu.resplen)
		LOG_FUNC_RETURN(card->ctx, SC_ERROR_INVALID_ARGUMENTS);

	data->modulus = (u8 *) malloc(len);
	if (!data->modulus)
		LOG_FUNC_RETURN(card->ctx, SC_ERROR_OUT_OF_MEMORY);

	memcpy(data->modulus, rbuf, len);

	LOG_FUNC_RETURN(card->ctx, SC_SUCCESS);
}