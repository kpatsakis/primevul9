epass2003_set_security_env(struct sc_card *card, const sc_security_env_t * env, int se_num)
{
	struct sc_apdu apdu;
	u8 sbuf[SC_MAX_APDU_BUFFER_SIZE] = { 0 };
	u8 *p;
	unsigned short fid = 0;
	int r, locked = 0;
	epass2003_exdata *exdata = NULL;

	if (!card->drv_data)
		return SC_ERROR_INVALID_ARGUMENTS;

	exdata = (epass2003_exdata *)card->drv_data;

	sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0x22, 0x41, 0);
    
	p = sbuf;
	*p++ = 0x80;		/* algorithm reference */
	*p++ = 0x01;
	*p++ = 0x84;

	*p++ = 0x81;
	*p++ = 0x02;

	fid = 0x2900;
	fid += (unsigned short)(0x20 * (env->key_ref[0] & 0xff));
	*p++ = fid >> 8;
	*p++ = fid & 0xff;
	r = p - sbuf;
	apdu.lc = r;
	apdu.datalen = r;
	apdu.data = sbuf;

	if (env->algorithm == SC_ALGORITHM_EC)
	{
		apdu.p2 = 0xB6;
		exdata->currAlg = SC_ALGORITHM_EC;
		if(env->algorithm_flags & SC_ALGORITHM_ECDSA_HASH_SHA1)
		{
			sbuf[2] = 0x91;
			exdata->ecAlgFlags = SC_ALGORITHM_ECDSA_HASH_SHA1;
		}
		else if (env->algorithm_flags & SC_ALGORITHM_ECDSA_HASH_SHA256)
		{
			sbuf[2] = 0x92;
			exdata->ecAlgFlags = SC_ALGORITHM_ECDSA_HASH_SHA256;
		}
		else
		{
			sc_log(card->ctx, "%0x Alg Not Support! ", env->algorithm_flags);
			goto err;
		}
	}
	else if(env->algorithm == SC_ALGORITHM_RSA)
	{
		exdata->currAlg = SC_ALGORITHM_RSA; 
		apdu.p2 = 0xB8;
		sc_log(card->ctx, "setenv RSA Algorithm alg_flags = %0x\n",env->algorithm_flags);
	}
	else
	{
		sc_log(card->ctx, "%0x Alg Not Support! ", env->algorithm);
	}

	if (se_num > 0) {
		r = sc_lock(card);
		LOG_TEST_RET(card->ctx, r, "sc_lock() failed");
		locked = 1;
	}
	if (apdu.datalen != 0) {
		r = sc_transmit_apdu_t(card, &apdu);
		if (r) {
			sc_log(card->ctx, "%s: APDU transmit failed", sc_strerror(r));
			goto err;
		}

		r = sc_check_sw(card, apdu.sw1, apdu.sw2);
		if (r) {
			sc_log(card->ctx, "%s: Card returned error", sc_strerror(r));
			goto err;
		}
	}
	if (se_num <= 0)
		return 0;

	sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0x22, 0xF2, se_num);
	r = sc_transmit_apdu_t(card, &apdu);
	sc_unlock(card);

	LOG_TEST_RET(card->ctx, r, "APDU transmit failed");
	return sc_check_sw(card, apdu.sw1, apdu.sw2);

err:
	if (locked)
		sc_unlock(card);
	return r;
}