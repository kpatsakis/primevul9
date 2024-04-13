static int sc_pkcs15emu_sc_hsm_add_dcod(sc_pkcs15_card_t * p15card, u8 id) {

	sc_card_t *card = p15card->card;
	sc_pkcs15_data_info_t *data_info;
	sc_pkcs15_object_t data_obj;
	u8 fid[2];
	u8 efbin[512];
	const u8 *ptr;
	size_t len;
	int r;

	fid[0] = DCOD_PREFIX;
	fid[1] = id;

	/* Try to select a related EF containing the PKCS#15 description of the data */
	len = sizeof efbin;
	r = read_file(p15card, fid, efbin, &len, 1);
	LOG_TEST_RET(card->ctx, r, "Skipping optional EF.DCOD");

	ptr = efbin;

	memset(&data_obj, 0, sizeof(data_obj));
	r = sc_pkcs15_decode_dodf_entry(p15card, &data_obj, &ptr, &len);
	LOG_TEST_RET(card->ctx, r, "Could not decode optional EF.DCOD");

	data_info = (sc_pkcs15_data_info_t *)data_obj.data;

	r = sc_pkcs15emu_add_data_object(p15card, &data_obj, data_info);

	LOG_TEST_RET(card->ctx, r, "Could not add data object to framework");

	return SC_SUCCESS;
}