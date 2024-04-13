static int gen_d(RSA *rsa)
{
	BN_CTX *bnctx;
	BIGNUM *r0, *r1, *r2;
	const BIGNUM *rsa_p, *rsa_q, *rsa_n, *rsa_e, *rsa_d;
	BIGNUM *rsa_n_new, *rsa_e_new, *rsa_d_new;

	bnctx = BN_CTX_new();
	if (bnctx == NULL)
		return -1;
	BN_CTX_start(bnctx);
	r0 = BN_CTX_get(bnctx);
	r1 = BN_CTX_get(bnctx);
	r2 = BN_CTX_get(bnctx);
	RSA_get0_key(rsa, &rsa_n, &rsa_e, &rsa_d);
	RSA_get0_factors(rsa, &rsa_p, &rsa_q);

	BN_sub(r1, rsa_p, BN_value_one());
	BN_sub(r2, rsa_q, BN_value_one());
	BN_mul(r0, r1, r2, bnctx);
	if ((rsa_d_new = BN_mod_inverse(NULL, rsa_e, r0, bnctx)) == NULL) {
		fprintf(stderr, "BN_mod_inverse() failed.\n");
		return -1;
	}

	/* RSA_set0_key will free previous value, and replace with new value
	 * Thus the need to copy the contents of rsa_n and rsa_e
	 */
	rsa_n_new = BN_dup(rsa_n);
	rsa_e_new = BN_dup(rsa_e);
	if (RSA_set0_key(rsa, rsa_n_new, rsa_e_new, rsa_d_new) != 1)
		return -1;

	BN_CTX_end(bnctx);
	BN_CTX_free(bnctx);
	return 0;
}