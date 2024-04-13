static int sc_pkcs15emu_gemsafeV1_init( sc_pkcs15_card_t *p15card)
{
	int		    r;
	unsigned int    i;
	struct sc_path  path;
	struct sc_file *file = NULL;
	struct sc_card *card = p15card->card;
	struct sc_apdu  apdu;
	u8		    rbuf[SC_MAX_APDU_BUFFER_SIZE];

	sc_log(p15card->card->ctx, "Setting pkcs15 parameters");

	if (p15card->tokeninfo->label)
		free(p15card->tokeninfo->label);
	p15card->tokeninfo->label = malloc(strlen(APPLET_NAME) + 1);
	if (!p15card->tokeninfo->label)
		return SC_ERROR_INTERNAL;
	strcpy(p15card->tokeninfo->label, APPLET_NAME);

	if (p15card->tokeninfo->serial_number)
		free(p15card->tokeninfo->serial_number);
	p15card->tokeninfo->serial_number = malloc(strlen(DRIVER_SERIAL_NUMBER) + 1);
	if (!p15card->tokeninfo->serial_number)
		return SC_ERROR_INTERNAL;
	strcpy(p15card->tokeninfo->serial_number, DRIVER_SERIAL_NUMBER);

	/* the GemSAFE applet version number */
	sc_format_apdu(card, &apdu, SC_APDU_CASE_2_SHORT, 0xca, 0xdf, 0x03);
	apdu.cla = 0x80;
	apdu.resp = rbuf;
	apdu.resplen = sizeof(rbuf);
	/* Manual says Le=0x05, but should be 0x08 to return full version number */
	apdu.le = 0x08;
	apdu.lc = 0;
	apdu.datalen = 0;
	r = sc_transmit_apdu(card, &apdu);
	LOG_TEST_RET(card->ctx, r, "APDU transmit failed");

	if (apdu.sw1 != 0x90 || apdu.sw2 != 0x00)
		return SC_ERROR_INTERNAL;
	if (r != SC_SUCCESS)
		return SC_ERROR_INTERNAL;

	/* the manufacturer ID, in this case GemPlus */
	if (p15card->tokeninfo->manufacturer_id)
		free(p15card->tokeninfo->manufacturer_id);
	p15card->tokeninfo->manufacturer_id = malloc(strlen(MANU_ID) + 1);
	if (!p15card->tokeninfo->manufacturer_id)
		return SC_ERROR_INTERNAL;
	strcpy(p15card->tokeninfo->manufacturer_id, MANU_ID);

	/* determine allocated key containers and length of certificates */
	r = gemsafe_get_cert_len(card);
	if (r != SC_SUCCESS)
		return SC_ERROR_INTERNAL;

	/* set certs */
	sc_log(p15card->card->ctx, "Setting certificates");
	for (i = 0; i < gemsafe_cert_max; i++) {
		struct sc_pkcs15_id p15Id;
		struct sc_path path;

		if (gemsafe_cert[i].label == NULL)
			continue;
		sc_format_path(gemsafe_cert[i].path, &path);
		sc_pkcs15_format_id(gemsafe_cert[i].id, &p15Id);
		path.index = gemsafe_cert[i].index;
		path.count = gemsafe_cert[i].count;
		sc_pkcs15emu_add_cert(p15card, SC_PKCS15_TYPE_CERT_X509,
				      gemsafe_cert[i].authority, &path, &p15Id,
				      gemsafe_cert[i].label, gemsafe_cert[i].obj_flags);
	}

	/* set gemsafe_pin */
	sc_log(p15card->card->ctx, "Setting PIN");
	for (i=0; i < gemsafe_pin_max; i++) {
		struct sc_pkcs15_id	p15Id;
		struct sc_path path;

		sc_pkcs15_format_id(gemsafe_pin[i].id, &p15Id);
		sc_format_path(gemsafe_pin[i].path, &path);
		if (gemsafe_pin[i].atr_len == 0 ||
		   (gemsafe_pin[i].atr_len == p15card->card->atr.len &&
		    memcmp(p15card->card->atr.value, gemsafe_pin[i].atr,
			   p15card->card->atr.len) == 0)) {
			sc_pkcs15emu_add_pin(p15card, &p15Id, gemsafe_pin[i].label,
					     &path, gemsafe_pin[i].ref, gemsafe_pin[i].type,
					     gemsafe_pin[i].minlen, gemsafe_pin[i].maxlen,
					     gemsafe_pin[i].flags, gemsafe_pin[i].tries_left,
					     gemsafe_pin[i].pad_char, gemsafe_pin[i].obj_flags);
			break;
		}
	};

	/* set private keys */
	sc_log(p15card->card->ctx, "Setting private keys");
	for (i = 0; i < gemsafe_cert_max; i++) {
		struct sc_pkcs15_id p15Id, authId, *pauthId;
		struct sc_path path;
		int key_ref = 0x03;

		if (gemsafe_prkeys[i].label == NULL)
			continue;
		sc_pkcs15_format_id(gemsafe_prkeys[i].id, &p15Id);
		if (gemsafe_prkeys[i].auth_id) {
			sc_pkcs15_format_id(gemsafe_prkeys[i].auth_id, &authId);
			pauthId = &authId;
		} else
			pauthId = NULL;
		sc_format_path(gemsafe_prkeys[i].path, &path);
		/*
		 * The key ref may be different for different sites;
		 * by adding flags=n where the low order 4 bits can be
		 * the key ref we can force it.
		 */
		if ( p15card->card->flags & 0x0F) {
			key_ref = p15card->card->flags & 0x0F;
			sc_debug(p15card->card->ctx, SC_LOG_DEBUG_NORMAL,
				 "Overriding key_ref %d with %d\n",
				 gemsafe_prkeys[i].ref, key_ref);
		} else
			key_ref = gemsafe_prkeys[i].ref;
		sc_pkcs15emu_add_prkey(p15card, &p15Id, gemsafe_prkeys[i].label,
				       SC_PKCS15_TYPE_PRKEY_RSA,
				       gemsafe_prkeys[i].modulus_len, gemsafe_prkeys[i].usage,
				       &path, key_ref, pauthId,
				       gemsafe_prkeys[i].obj_flags);
	}

	/* select the application DF */
	sc_log(p15card->card->ctx, "Selecting application DF");
	sc_format_path(GEMSAFE_APP_PATH, &path);
	r = sc_select_file(card, &path, &file);
	if (r != SC_SUCCESS || !file)
		return SC_ERROR_INTERNAL;
	/* set the application DF */
	if (p15card->file_app)
		free(p15card->file_app);
	p15card->file_app = file;

	return SC_SUCCESS;
}