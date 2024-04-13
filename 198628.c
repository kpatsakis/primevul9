int sc_pkcs15emu_sc_hsm_get_public_key(struct sc_context *ctx, sc_cvc_t *cvc, struct sc_pkcs15_pubkey *pubkey)
{
	if (cvc->publicPoint && cvc->publicPointlen) {
		return sc_pkcs15emu_sc_hsm_get_ec_public_key(ctx, cvc, pubkey);
	} else {
		return sc_pkcs15emu_sc_hsm_get_rsa_public_key(ctx, cvc, pubkey);
	}
}