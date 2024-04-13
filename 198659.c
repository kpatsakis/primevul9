static int epass2003_decipher(struct sc_card *card, const u8 * data, size_t datalen,
		u8 * out, size_t outlen)
{
	int r;
	struct sc_apdu apdu;
	u8 rbuf[SC_MAX_APDU_BUFFER_SIZE] = { 0 };
	u8 sbuf[SC_MAX_APDU_BUFFER_SIZE] = { 0 };
	epass2003_exdata *exdata = NULL;
	
	LOG_FUNC_CALLED(card->ctx);
	
	if (!card->drv_data)
		return SC_ERROR_INVALID_ARGUMENTS;
	
	exdata = (epass2003_exdata *)card->drv_data;

	if(exdata->currAlg == SC_ALGORITHM_EC)
	{
		if(exdata->ecAlgFlags & SC_ALGORITHM_ECDSA_HASH_SHA1)
		{
			r = hash_data(data, datalen, sbuf, SC_ALGORITHM_ECDSA_HASH_SHA1);
			LOG_TEST_RET(card->ctx, r, "hash_data failed"); 
			sc_format_apdu(card, &apdu, SC_APDU_CASE_3,0x2A, 0x9E, 0x9A);
			apdu.data = sbuf;
			apdu.lc = 0x14;
			apdu.datalen = 0x14;
		}
		else if (exdata->ecAlgFlags & SC_ALGORITHM_ECDSA_HASH_SHA256)
		{
			r = hash_data(data, datalen, sbuf, SC_ALGORITHM_ECDSA_HASH_SHA256);
			LOG_TEST_RET(card->ctx, r, "hash_data failed");
			sc_format_apdu(card, &apdu, SC_APDU_CASE_3,0x2A, 0x9E, 0x9A);
			apdu.data = sbuf;
			apdu.lc = 0x20;
			apdu.datalen = 0x20;
		}
		else
		{
			return SC_ERROR_NOT_SUPPORTED;
		}
		apdu.resp = rbuf;
		apdu.resplen = sizeof(rbuf);
		apdu.le = 0;

		r = sc_transmit_apdu_t(card, &apdu);
		LOG_TEST_RET(card->ctx, r, "APDU transmit failed");
		if (apdu.sw1 == 0x90 && apdu.sw2 == 0x00) {
			size_t len = apdu.resplen > outlen ? outlen : apdu.resplen;
			memcpy(out, apdu.resp, len);
			LOG_FUNC_RETURN(card->ctx, len);
		}
		LOG_FUNC_RETURN(card->ctx, sc_check_sw(card, apdu.sw1, apdu.sw2));
	}
	else if(exdata->currAlg == SC_ALGORITHM_RSA)
	{
		sc_format_apdu(card, &apdu, SC_APDU_CASE_4_EXT, 0x2A, 0x80, 0x86);
		apdu.resp = rbuf;
		apdu.resplen = sizeof(rbuf);
		apdu.le = 0;

		memcpy(sbuf, data, datalen);
		apdu.data = sbuf;
		apdu.lc = datalen;
		apdu.datalen = datalen;
	}
	else
	{
		sc_format_apdu(card, &apdu, SC_APDU_CASE_4_EXT, 0x2A, 0x80, 0x86);
		apdu.resp = rbuf;
		apdu.resplen = sizeof(rbuf);
		apdu.le = 256;

		memcpy(sbuf, data, datalen);
		apdu.data = sbuf;
		apdu.lc = datalen;
		apdu.datalen = datalen;
	}

	r = sc_transmit_apdu_t(card, &apdu);
	LOG_TEST_RET(card->ctx, r, "APDU transmit failed");

	if (apdu.sw1 == 0x90 && apdu.sw2 == 0x00) {
		size_t len = apdu.resplen > outlen ? outlen : apdu.resplen;
		memcpy(out, apdu.resp, len);
		LOG_FUNC_RETURN(card->ctx, len);
	}

	LOG_FUNC_RETURN(card->ctx, sc_check_sw(card, apdu.sw1, apdu.sw2));
}