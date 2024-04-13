static int cac_set_security_env(sc_card_t *card, const sc_security_env_t *env, int se_num)
{
	int r = SC_SUCCESS;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);

	sc_debug(card->ctx, SC_LOG_DEBUG_NORMAL,
		 "flags=%08lx op=%d alg=%d algf=%08x algr=%08x kr0=%02x, krfl=%"SC_FORMAT_LEN_SIZE_T"u\n",
		 env->flags, env->operation, env->algorithm,
		 env->algorithm_flags, env->algorithm_ref, env->key_ref[0],
		 env->key_ref_len);

	if (env->algorithm != SC_ALGORITHM_RSA) {
		 r = SC_ERROR_NO_CARD_SUPPORT;
	}


	SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_VERBOSE, r);
}