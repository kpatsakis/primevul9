static int setcos_set_security_env(sc_card_t *card,
				   const sc_security_env_t *env, int se_num)
{
	if (env->flags & SC_SEC_ENV_ALG_PRESENT) {
		sc_security_env_t tmp;

		tmp = *env;
		tmp.flags &= ~SC_SEC_ENV_ALG_PRESENT;
		tmp.flags |= SC_SEC_ENV_ALG_REF_PRESENT;
		if (tmp.algorithm != SC_ALGORITHM_RSA) {
			sc_log(card->ctx,  "Only RSA algorithm supported.\n");
			return SC_ERROR_NOT_SUPPORTED;
		}
		switch (card->type) {
		case SC_CARD_TYPE_SETCOS_PKI:
		case SC_CARD_TYPE_SETCOS_FINEID:
		case SC_CARD_TYPE_SETCOS_FINEID_V2_2048:
		case SC_CARD_TYPE_SETCOS_NIDEL:
		case SC_CARD_TYPE_SETCOS_44:
		case SC_CARD_TYPE_SETCOS_EID_V2_0:
		case SC_CARD_TYPE_SETCOS_EID_V2_1:
			break;
		default:
			sc_log(card->ctx,  "Card does not support RSA.\n");
			return SC_ERROR_NOT_SUPPORTED;
			break;
		}
		tmp.algorithm_ref = 0x00;
		/* potential FIXME: return an error, if an unsupported
		 * pad or hash was requested, although this shouldn't happen.
		 */
		if (env->algorithm_flags & SC_ALGORITHM_RSA_PAD_PKCS1)
			tmp.algorithm_ref = 0x02;
		if (tmp.algorithm_flags & SC_ALGORITHM_RSA_HASH_SHA1)
			tmp.algorithm_ref |= 0x10;
		return setcos_set_security_env2(card, &tmp, se_num);
	}
	return setcos_set_security_env2(card, env, se_num);
}