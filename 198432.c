static int sc_pkcs15emu_sc_hsm_get_rsa_public_key(struct sc_context *ctx, sc_cvc_t *cvc, struct sc_pkcs15_pubkey *pubkey)
{
	pubkey->algorithm = SC_ALGORITHM_RSA;

	pubkey->alg_id = (struct sc_algorithm_id *)calloc(1, sizeof(struct sc_algorithm_id));
	if (!pubkey->alg_id)
		return SC_ERROR_OUT_OF_MEMORY;

	pubkey->alg_id->algorithm = SC_ALGORITHM_RSA;

	pubkey->u.rsa.modulus.len	= cvc->primeOrModuluslen;
	pubkey->u.rsa.modulus.data	= malloc(pubkey->u.rsa.modulus.len);
	pubkey->u.rsa.exponent.len	= cvc->coefficientAorExponentlen;
	pubkey->u.rsa.exponent.data	= malloc(pubkey->u.rsa.exponent.len);
	if (!pubkey->u.rsa.modulus.data || !pubkey->u.rsa.exponent.data)
		return SC_ERROR_OUT_OF_MEMORY;

	memcpy(pubkey->u.rsa.exponent.data, cvc->coefficientAorExponent, pubkey->u.rsa.exponent.len);
	memcpy(pubkey->u.rsa.modulus.data, cvc->primeOrModulus, pubkey->u.rsa.modulus.len);

	return SC_SUCCESS;
}