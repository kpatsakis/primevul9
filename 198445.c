static int sc_pkcs15emu_sc_hsm_add_cd(sc_pkcs15_card_t * p15card, u8 id) {

	sc_card_t *card = p15card->card;
	sc_pkcs15_cert_info_t *cert_info;
	sc_pkcs15_object_t obj;
	u8 fid[2];
	u8 efbin[512];
	const u8 *ptr;
	size_t len;
	int r;

	fid[0] = CD_PREFIX;
	fid[1] = id;

	/* Try to select a related EF containing the PKCS#15 description of the data */
	len = sizeof efbin;
	r = read_file(p15card, fid, efbin, &len, 1);
	LOG_TEST_RET(card->ctx, r, "Skipping optional EF.DCOD");

	ptr = efbin;

	memset(&obj, 0, sizeof(obj));
	r = sc_pkcs15_decode_cdf_entry(p15card, &obj, &ptr, &len);
	LOG_TEST_RET(card->ctx, r, "Skipping optional EF.CDOD");

	cert_info = (sc_pkcs15_cert_info_t *)obj.data;

	r = sc_pkcs15emu_add_x509_cert(p15card, &obj, cert_info);

	LOG_TEST_RET(card->ctx, r, "Could not add data object to framework");

	return SC_SUCCESS;
}