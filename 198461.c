static int tcos_set_security_env(sc_card_t *card, const sc_security_env_t *env, int se_num)
{
	sc_context_t *ctx;
	sc_apdu_t apdu;
	u8 sbuf[SC_MAX_APDU_BUFFER_SIZE], *p;
	int r, default_key, tcos3;
	tcos_data *data;

	assert(card != NULL && env != NULL);
	ctx = card->ctx;
	tcos3=(card->type==SC_CARD_TYPE_TCOS_V3);
	data=(tcos_data *)card->drv_data;

        if (se_num || (env->operation!=SC_SEC_OPERATION_DECIPHER && env->operation!=SC_SEC_OPERATION_SIGN)){
		SC_FUNC_RETURN(ctx, SC_LOG_DEBUG_NORMAL, SC_ERROR_INVALID_ARGUMENTS);
	}
	if(!(env->flags & SC_SEC_ENV_KEY_REF_PRESENT))
		sc_debug(ctx, SC_LOG_DEBUG_NORMAL,
			"No Key-Reference in SecEnvironment\n");
	else
		sc_debug(ctx, SC_LOG_DEBUG_NORMAL,
			 "Key-Reference %02X (len=%"SC_FORMAT_LEN_SIZE_T"u)\n",
			 env->key_ref[0], env->key_ref_len);
	/* Key-Reference 0x80 ?? */
	default_key= !(env->flags & SC_SEC_ENV_KEY_REF_PRESENT) || (env->key_ref_len==1 && env->key_ref[0]==0x80);
	sc_debug(ctx, SC_LOG_DEBUG_NORMAL,
		"TCOS3:%d PKCS1:%d\n", tcos3,
		!!(env->algorithm_flags & SC_ALGORITHM_RSA_PAD_PKCS1));

	data->pad_flags = env->algorithm_flags;
	data->next_sign = default_key;

	sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0x22, tcos3 ? 0x41 : 0xC1, 0xB8);
	p = sbuf;
	*p++=0x80; *p++=0x01; *p++=tcos3 ? 0x0A : 0x10;
	if (env->flags & SC_SEC_ENV_KEY_REF_PRESENT) {
		*p++ = (env->flags & SC_SEC_ENV_KEY_REF_SYMMETRIC) ? 0x83 : 0x84;
		*p++ = env->key_ref_len;
		memcpy(p, env->key_ref, env->key_ref_len);
		p += env->key_ref_len;
	}
	apdu.data = sbuf;
	apdu.lc = apdu.datalen = (p - sbuf);

	r=sc_transmit_apdu(card, &apdu);
	if (r) {
		sc_debug(ctx, SC_LOG_DEBUG_NORMAL,
			"%s: APDU transmit failed", sc_strerror(r));
		return r;
	}
	if (apdu.sw1==0x6A && (apdu.sw2==0x81 || apdu.sw2==0x88)) {
		sc_debug(ctx, SC_LOG_DEBUG_NORMAL,
			"Detected Signature-Only key\n");
		if (env->operation==SC_SEC_OPERATION_SIGN && default_key) return SC_SUCCESS;
	}
	SC_FUNC_RETURN(ctx, SC_LOG_DEBUG_VERBOSE, sc_check_sw(card, apdu.sw1, apdu.sw2));
}