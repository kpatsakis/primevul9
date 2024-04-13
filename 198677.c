static int sc_pkcs15emu_sc_hsm_get_ec_public_key(struct sc_context *ctx, sc_cvc_t *cvc, struct sc_pkcs15_pubkey *pubkey)
{
	struct sc_ec_parameters *ecp;
	const struct sc_lv_data *oid;
	int r;

	pubkey->algorithm = SC_ALGORITHM_EC;

	r = sc_pkcs15emu_sc_hsm_get_curve_oid(cvc, &oid);
	if (r != SC_SUCCESS)
		return r;

	ecp = calloc(1, sizeof(struct sc_ec_parameters));
	if (!ecp)
		return SC_ERROR_OUT_OF_MEMORY;

	ecp->der.len = oid->len + 2;
	ecp->der.value = calloc(ecp->der.len, 1);
	if (!ecp->der.value) {
		free(ecp);
		return SC_ERROR_OUT_OF_MEMORY;
	}

	*(ecp->der.value + 0) = 0x06;
	*(ecp->der.value + 1) = (u8)oid->len;
	memcpy(ecp->der.value + 2, oid->value, oid->len);
	ecp->type = 1;		// Named curve

	pubkey->alg_id = (struct sc_algorithm_id *)calloc(1, sizeof(struct sc_algorithm_id));
	if (!pubkey->alg_id) {
		free(ecp->der.value);
		free(ecp);
		return SC_ERROR_OUT_OF_MEMORY;
	}

	pubkey->alg_id->algorithm = SC_ALGORITHM_EC;
	pubkey->alg_id->params = ecp;

	pubkey->u.ec.ecpointQ.value = malloc(cvc->publicPointlen);
	if (!pubkey->u.ec.ecpointQ.value)
		return SC_ERROR_OUT_OF_MEMORY;
	memcpy(pubkey->u.ec.ecpointQ.value, cvc->publicPoint, cvc->publicPointlen);
	pubkey->u.ec.ecpointQ.len = cvc->publicPointlen;

	pubkey->u.ec.params.der.value = malloc(ecp->der.len);
	if (!pubkey->u.ec.params.der.value)
		return SC_ERROR_OUT_OF_MEMORY;
	memcpy(pubkey->u.ec.params.der.value, ecp->der.value, ecp->der.len);
	pubkey->u.ec.params.der.len = ecp->der.len;

	/* FIXME: check return value? */
	sc_pkcs15_fix_ec_parameters(ctx, &pubkey->u.ec.params);

	return SC_SUCCESS;
}