static int cac_parse_aid(sc_card_t *card, cac_private_data_t *priv, u8 *aid, int aid_len)
{
	cac_object_t new_object;
	cac_properties_t prop;
	size_t i;
	int r;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);

	/* Search for PKI applets (7 B). Ignore generic objects for now */
	if (aid_len != 7 || (memcmp(aid, CAC_1_RID "\x01", 6) != 0
	    && memcmp(aid, CAC_1_RID "\x00", 6) != 0))
		return SC_SUCCESS;

	sc_mem_clear(&new_object.path, sizeof(sc_path_t));
	memcpy(new_object.path.aid.value, aid, aid_len);
	new_object.path.aid.len = aid_len;

	/* Call without OID set will just select the AID without subseqent
	 * OID selection, which we need to figure out just now
	 */
	cac_select_file_by_type(card, &new_object.path, NULL, SC_CARD_TYPE_CAC_II);
	r = cac_get_properties(card, &prop);
	if (r < 0)
		return SC_ERROR_INTERNAL;

	for (i = 0; i < prop.num_objects; i++) {
		/* don't fail just because we have more certs than we can support */
		if (priv->cert_next >= MAX_CAC_SLOTS)
			return SC_SUCCESS;

		sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE,
		    "ACA: pki_object found, cert_next=%d (%s), privkey=%d",
		    priv->cert_next, cac_labels[priv->cert_next],
		    prop.objects[i].privatekey);

		/* If the private key is not initialized, we can safely
		 * ignore this object here, but increase the pointer to follow
		 * the certificate labels
		 */
		if (!prop.objects[i].privatekey) {
			priv->cert_next++;
			continue;
		}

		/* OID here has always 2B */
		memcpy(new_object.path.value, &prop.objects[i].oid, 2);
		new_object.path.len = 2;
		new_object.path.type = SC_PATH_TYPE_FILE_ID;
		new_object.name = cac_labels[priv->cert_next];
		new_object.fd = priv->cert_next+1;
		cac_add_object_to_list(&priv->pki_list, &new_object);
		priv->cert_next++;
	}

	return SC_SUCCESS;
}