static int sc_pkcs15emu_sc_hsm_add_prkd(sc_pkcs15_card_t * p15card, u8 keyid) {

	sc_card_t *card = p15card->card;
	sc_pkcs15_cert_info_t cert_info;
	sc_pkcs15_object_t cert_obj;
	struct sc_pkcs15_object prkd;
	sc_pkcs15_prkey_info_t *key_info;
	u8 fid[2];
	/* enough to hold a complete certificate */
	u8 efbin[4096];
	u8 *ptr;
	size_t len;
	int r;

	fid[0] = PRKD_PREFIX;
	fid[1] = keyid;

	/* Try to select a related EF containing the PKCS#15 description of the key */
	len = sizeof efbin;
	r = read_file(p15card, fid, efbin, &len, 1);
	LOG_TEST_RET(card->ctx, r, "Skipping optional EF.PRKD");

	ptr = efbin;

	memset(&prkd, 0, sizeof(prkd));
	r = sc_pkcs15_decode_prkdf_entry(p15card, &prkd, (const u8 **)&ptr, &len);
	LOG_TEST_RET(card->ctx, r, "Skipping optional EF.PRKD");

	/* All keys require user PIN authentication */
	prkd.auth_id.len = 1;
	prkd.auth_id.value[0] = 1;

	/*
	 * Set private key flag as all keys are private anyway
	 */
	prkd.flags |= SC_PKCS15_CO_FLAG_PRIVATE;

	key_info = (sc_pkcs15_prkey_info_t *)prkd.data;
	key_info->key_reference = keyid;
	key_info->path.aid.len = 0;

	if (prkd.type == SC_PKCS15_TYPE_PRKEY_RSA) {
		r = sc_pkcs15emu_add_rsa_prkey(p15card, &prkd, key_info);
	} else {
		r = sc_pkcs15emu_add_ec_prkey(p15card, &prkd, key_info);
	}

	LOG_TEST_RET(card->ctx, r, "Could not add private key to framework");

	/* Check if we also have a certificate for the private key */
	fid[0] = EE_CERTIFICATE_PREFIX;

	len = sizeof efbin;
	r = read_file(p15card, fid, efbin, &len, 0);
	LOG_TEST_RET(card->ctx, r, "Could not read EF");

	if (efbin[0] == 0x67) {		/* Decode CSR and create public key object */
		sc_pkcs15emu_sc_hsm_add_pubkey(p15card, efbin, len, key_info, prkd.label);
		free(key_info);
		return SC_SUCCESS;		/* Ignore any errors */
	}

	if (efbin[0] != 0x30) {
		free(key_info);
		return SC_SUCCESS;
	}

	memset(&cert_info, 0, sizeof(cert_info));
	memset(&cert_obj, 0, sizeof(cert_obj));

	cert_info.id = key_info->id;
	sc_path_set(&cert_info.path, SC_PATH_TYPE_FILE_ID, fid, 2, 0, 0);
	cert_info.path.count = -1;
	if (p15card->opts.use_file_cache) {
		/* look this up with our AID, which should already be cached from the
		 * call to `read_file`. This may have the side effect that OpenSC's
		 * caching layer re-selects our applet *if the cached file cannot be
		 * found/used* and we may loose the authentication status. We assume
		 * that caching works perfectly without this side effect. */
		cert_info.path.aid = sc_hsm_aid;
	}

	strlcpy(cert_obj.label, prkd.label, sizeof(cert_obj.label));
	r = sc_pkcs15emu_add_x509_cert(p15card, &cert_obj, &cert_info);

	free(key_info);

	LOG_TEST_RET(card->ctx, r, "Could not add certificate");

	return SC_SUCCESS;
}