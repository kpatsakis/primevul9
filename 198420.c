static int sc_pkcs15emu_sc_hsm_add_pubkey(sc_pkcs15_card_t *p15card, u8 *efbin, size_t len, sc_pkcs15_prkey_info_t *key_info, char *label)
{
	struct sc_context *ctx = p15card->card->ctx;
	sc_card_t *card = p15card->card;
	sc_pkcs15_pubkey_info_t pubkey_info;
	sc_pkcs15_object_t pubkey_obj;
	struct sc_pkcs15_pubkey pubkey;
	sc_cvc_t cvc;
	u8 *cvcpo;
	int r;

	cvcpo = efbin;

	memset(&cvc, 0, sizeof(cvc));
	r = sc_pkcs15emu_sc_hsm_decode_cvc(p15card, (const u8 **)&cvcpo, &len, &cvc);
	LOG_TEST_RET(ctx, r, "Could decode certificate signing request");

	memset(&pubkey, 0, sizeof(pubkey));
	r = sc_pkcs15emu_sc_hsm_get_public_key(ctx, &cvc, &pubkey);
	LOG_TEST_RET(card->ctx, r, "Could not extract public key");

	memset(&pubkey_info, 0, sizeof(pubkey_info));
	memset(&pubkey_obj, 0, sizeof(pubkey_obj));

	r = sc_pkcs15_encode_pubkey(ctx, &pubkey, &pubkey_obj.content.value, &pubkey_obj.content.len);
	LOG_TEST_RET(ctx, r, "Could not encode public key");
	r = sc_pkcs15_encode_pubkey(ctx, &pubkey, &pubkey_info.direct.raw.value, &pubkey_info.direct.raw.len);
	LOG_TEST_RET(ctx, r, "Could not encode public key");
	r = sc_pkcs15_encode_pubkey_as_spki(ctx, &pubkey, &pubkey_info.direct.spki.value, &pubkey_info.direct.spki.len);
	LOG_TEST_RET(ctx, r, "Could not encode public key");

	pubkey_info.id = key_info->id;
	strlcpy(pubkey_obj.label, label, sizeof(pubkey_obj.label));

	if (pubkey.algorithm == SC_ALGORITHM_RSA) {
		pubkey_info.modulus_length = pubkey.u.rsa.modulus.len << 3;
		pubkey_info.usage = SC_PKCS15_PRKEY_USAGE_ENCRYPT|SC_PKCS15_PRKEY_USAGE_VERIFY|SC_PKCS15_PRKEY_USAGE_WRAP;
		r = sc_pkcs15emu_add_rsa_pubkey(p15card, &pubkey_obj, &pubkey_info);
	} else {
		/* TODO fix if support of non multiple of 8 curves are added */
		pubkey_info.field_length = cvc.primeOrModuluslen << 3;
		pubkey_info.usage = SC_PKCS15_PRKEY_USAGE_VERIFY;
		r = sc_pkcs15emu_add_ec_pubkey(p15card, &pubkey_obj, &pubkey_info);
	}
	LOG_TEST_RET(ctx, r, "Could not add public key");

	sc_pkcs15emu_sc_hsm_free_cvc(&cvc);
	sc_pkcs15_erase_pubkey(&pubkey);

	return SC_SUCCESS;
}