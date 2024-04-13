static int cac_populate_cac_alt(sc_card_t *card, int index, cac_private_data_t *priv)
{
	int r, i;
	cac_object_t pki_obj = cac_cac_pki_obj;
	u8 buf[100];
	u8 *val;
	size_t val_len;

	/* populate PKI objects */
	for (i = index; i < MAX_CAC_SLOTS; i++) {
		r = cac_select_pki_applet(card, i);
		if (r == SC_SUCCESS) {
			pki_obj.name = cac_labels[i];
			sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE,
			    "CAC: pki_object found, cert_next=%d (%s),", i, pki_obj.name);
			pki_obj.path.aid.value[pki_obj.path.aid.len-1] = i;
			pki_obj.fd = i+1; /* don't use id of zero */
			cac_add_object_to_list(&priv->pki_list, &pki_obj);
		}
	}

	/* populate non-PKI objects */
	for (i=0; i < cac_object_count; i++) {
		r = cac_select_file_by_type(card, &cac_objects[i].path, NULL,
		    SC_CARD_TYPE_CAC_II);
		if (r == SC_SUCCESS) {
			sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE,
			    "CAC: obj_object found, cert_next=%d (%s),",
			    i, cac_objects[i].name);
			cac_add_object_to_list(&priv->general_list, &cac_objects[i]);
		}
	}

	/*
	 * create a cuid to simulate the cac 2 cuid.
	 */
	priv->cuid = cac_cac_cuid;
	/* create a serial number by hashing the first 100 bytes of the
	 * first certificate on the card */
	r = cac_select_pki_applet(card, index);
	if (r < 0) {
		return r; /* shouldn't happen unless the card has been removed or is malfunctioning */
	}
	val = buf;
	val_len = cac_read_binary(card, 0, val, sizeof(buf), 0);
	if (val_len > 0) {
		priv->cac_id = malloc(20);
		if (priv->cac_id == NULL) {
			return SC_ERROR_OUT_OF_MEMORY;
		}
#ifdef ENABLE_OPENSSL
		SHA1(val, val_len, priv->cac_id);
		priv->cac_id_len = 20;
		sc_debug_hex(card->ctx, SC_LOG_DEBUG_VERBOSE,
		    "cuid", priv->cac_id, priv->cac_id_len);
#else
		sc_log(card->ctx, "OpenSSL Required");
		return SC_ERROR_NOT_SUPPORTED;
#endif /* ENABLE_OPENSSL */
	}
	return SC_SUCCESS;
}