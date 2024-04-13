static int setcos_set_security_env2(sc_card_t *card,
				    const sc_security_env_t *env, int se_num)
{
	sc_apdu_t apdu;
	u8 sbuf[SC_MAX_APDU_BUFFER_SIZE];
	u8 *p;
	int r, locked = 0;

	assert(card != NULL && env != NULL);

	if (card->type == SC_CARD_TYPE_SETCOS_44 ||
	    card->type == SC_CARD_TYPE_SETCOS_NIDEL ||
	    SETCOS_IS_EID_APPLET(card)) {
		if (env->flags & SC_SEC_ENV_KEY_REF_SYMMETRIC) {
			sc_log(card->ctx,  "symmetric keyref not supported.\n");
			return SC_ERROR_NOT_SUPPORTED;
		}
		if (se_num > 0) {
			sc_log(card->ctx,  "restore security environment not supported.\n");
			return SC_ERROR_NOT_SUPPORTED;
		}
	}

	sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0x22, 0, 0);
	switch (env->operation) {
	case SC_SEC_OPERATION_DECIPHER:
		/* Should be 0x81 */
		apdu.p1 = 0x41;
		apdu.p2 = 0xB8;
		break;
	case SC_SEC_OPERATION_SIGN:
		/* Should be 0x41 */
		apdu.p1 = ((card->type == SC_CARD_TYPE_SETCOS_FINEID_V2) ||
		           (card->type == SC_CARD_TYPE_SETCOS_FINEID_V2_2048) ||
		           (card->type == SC_CARD_TYPE_SETCOS_44) ||
			   (card->type == SC_CARD_TYPE_SETCOS_NIDEL) || 
			   SETCOS_IS_EID_APPLET(card)) ? 0x41 : 0x81;
		apdu.p2 = 0xB6;
		break;
	default:
		return SC_ERROR_INVALID_ARGUMENTS;
	}
	apdu.le = 0;
	p = sbuf;
	if (env->flags & SC_SEC_ENV_ALG_REF_PRESENT) {
		*p++ = 0x80;	/* algorithm reference */
		*p++ = 0x01;
		*p++ = env->algorithm_ref & 0xFF;
	}
	if (env->flags & SC_SEC_ENV_FILE_REF_PRESENT) {
		*p++ = 0x81;
		*p++ = env->file_ref.len;
		memcpy(p, env->file_ref.value, env->file_ref.len);
		p += env->file_ref.len;
	}
	if (env->flags & SC_SEC_ENV_KEY_REF_PRESENT &&
	    !(card->type == SC_CARD_TYPE_SETCOS_NIDEL ||
	      card->type == SC_CARD_TYPE_SETCOS_FINEID_V2_2048)) {
		if (env->flags & SC_SEC_ENV_KEY_REF_SYMMETRIC)
			*p++ = 0x83;
		else
			*p++ = 0x84;
		*p++ = env->key_ref_len;
		memcpy(p, env->key_ref, env->key_ref_len);
		p += env->key_ref_len;
	}
	r = p - sbuf;
	apdu.lc = r;
	apdu.datalen = r;
	apdu.data = sbuf;
	apdu.resplen = 0;
	if (se_num > 0) {
		r = sc_lock(card);
		LOG_TEST_RET(card->ctx, r, "sc_lock() failed");
		locked = 1;
	}
	if (apdu.datalen != 0) {
		r = sc_transmit_apdu(card, &apdu);
		if (r) {
			sc_log(card->ctx, 
				"%s: APDU transmit failed", sc_strerror(r));
			goto err;
		}
		r = sc_check_sw(card, apdu.sw1, apdu.sw2);
		if (r) {
			sc_log(card->ctx, 
				"%s: Card returned error", sc_strerror(r));
			goto err;
		}
	}
	if (se_num <= 0)
		return 0;
	sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0x22, 0xF2, se_num);
	r = sc_transmit_apdu(card, &apdu);
	sc_unlock(card);
	LOG_TEST_RET(card->ctx, r, "APDU transmit failed");
	return sc_check_sw(card, apdu.sw1, apdu.sw2);
err:
	if (locked)
		sc_unlock(card);
	return r;
}