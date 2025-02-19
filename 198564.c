sc_pkcs15emu_esteid_init (sc_pkcs15_card_t * p15card)
{
	sc_card_t *card = p15card->card;
	unsigned char buff[128];
	int r, i;
	size_t field_length = 0, modulus_length = 0;
	sc_path_t tmppath;

	set_string (&p15card->tokeninfo->label, "ID-kaart");
	set_string (&p15card->tokeninfo->manufacturer_id, "AS Sertifitseerimiskeskus");

	/* Select application directory */
	sc_format_path ("3f00eeee5044", &tmppath);
	r = sc_select_file (card, &tmppath, NULL);
	SC_TEST_RET(card->ctx, SC_LOG_DEBUG_NORMAL, r, "select esteid PD failed");

	/* read the serial (document number) */
	r = sc_read_record (card, SC_ESTEID_PD_DOCUMENT_NR, buff, sizeof(buff), SC_RECORD_BY_REC_NR);
	SC_TEST_RET(card->ctx, SC_LOG_DEBUG_NORMAL, r, "read document number failed");
	buff[MIN((size_t) r, (sizeof buff)-1)] = '\0';
	set_string (&p15card->tokeninfo->serial_number, (const char *) buff);

	p15card->tokeninfo->flags = SC_PKCS15_TOKEN_PRN_GENERATION
				  | SC_PKCS15_TOKEN_EID_COMPLIANT
				  | SC_PKCS15_TOKEN_READONLY;

	/* add certificates */
	for (i = 0; i < 2; i++) {
		static const char *esteid_cert_names[2] = {
			"Isikutuvastus",
			"Allkirjastamine"};
		static char const *esteid_cert_paths[2] = {
			"3f00eeeeaace",
			"3f00eeeeddce"};
		static int esteid_cert_ids[2] = {1, 2};

		struct sc_pkcs15_cert_info cert_info;
		struct sc_pkcs15_object cert_obj;

		memset(&cert_info, 0, sizeof(cert_info));
		memset(&cert_obj, 0, sizeof(cert_obj));

		cert_info.id.value[0] = esteid_cert_ids[i];
		cert_info.id.len = 1;
		sc_format_path(esteid_cert_paths[i], &cert_info.path);
		strlcpy(cert_obj.label, esteid_cert_names[i], sizeof(cert_obj.label));
		r = sc_pkcs15emu_add_x509_cert(p15card, &cert_obj, &cert_info);
		if (r < 0)
			return SC_ERROR_INTERNAL;
		if (i == 0) {
			sc_pkcs15_cert_t *cert = NULL;
			r = sc_pkcs15_read_certificate(p15card, &cert_info, &cert);
			if (r < 0)
				return SC_ERROR_INTERNAL;
			if (cert->key->algorithm == SC_ALGORITHM_EC)
				field_length = cert->key->u.ec.params.field_length;
			else
				modulus_length = cert->key->u.rsa.modulus.len * 8;
			if (r == SC_SUCCESS) {
				static const struct sc_object_id cn_oid = {{ 2, 5, 4, 3, -1 }};
				u8 *cn_name = NULL;
				size_t cn_len = 0;
				sc_pkcs15_get_name_from_dn(card->ctx, cert->subject,
					cert->subject_len, &cn_oid, &cn_name, &cn_len);
				if (cn_len > 0) {
					char *token_name = malloc(cn_len+1);
					if (token_name) {
						memcpy(token_name, cn_name, cn_len);
						token_name[cn_len] = '\0';
						set_string(&p15card->tokeninfo->label, (const char*)token_name);
						free(token_name);
					}
				}
				free(cn_name);
				sc_pkcs15_free_certificate(cert);
			}
		}
	}

	/* the file with key pin info (tries left) */
	sc_format_path ("3f000016", &tmppath);
	r = sc_select_file (card, &tmppath, NULL);
	if (r < 0)
		return SC_ERROR_INTERNAL;

	/* add pins */
	for (i = 0; i < 3; i++) {
		unsigned char tries_left;
		static const char *esteid_pin_names[3] = {
			"PIN1",
			"PIN2",
			"PUK" };
			
		static const int esteid_pin_min[3] = {4, 5, 8};
		static const int esteid_pin_ref[3] = {1, 2, 0};
		static const int esteid_pin_authid[3] = {1, 2, 3};
		static const int esteid_pin_flags[3] = {0, 0, SC_PKCS15_PIN_FLAG_UNBLOCKING_PIN};
		
		struct sc_pkcs15_auth_info pin_info;
		struct sc_pkcs15_object pin_obj;

		memset(&pin_info, 0, sizeof(pin_info));
		memset(&pin_obj, 0, sizeof(pin_obj));

		/* read the number of tries left for the PIN */
		r = sc_read_record (card, i + 1, buff, sizeof(buff), SC_RECORD_BY_REC_NR);
		if (r < 0)
			return SC_ERROR_INTERNAL;
		tries_left = buff[5];

		pin_info.auth_id.len = 1;
		pin_info.auth_id.value[0] = esteid_pin_authid[i];
		pin_info.auth_type = SC_PKCS15_PIN_AUTH_TYPE_PIN;
		pin_info.attrs.pin.reference = esteid_pin_ref[i];
		pin_info.attrs.pin.flags = esteid_pin_flags[i];
		pin_info.attrs.pin.type = SC_PKCS15_PIN_TYPE_ASCII_NUMERIC;
		pin_info.attrs.pin.min_length = esteid_pin_min[i];
		pin_info.attrs.pin.stored_length = 12;
		pin_info.attrs.pin.max_length = 12;
		pin_info.attrs.pin.pad_char = '\0';
		pin_info.tries_left = (int)tries_left;
		pin_info.max_tries = 3;

		strlcpy(pin_obj.label, esteid_pin_names[i], sizeof(pin_obj.label));
		pin_obj.flags = esteid_pin_flags[i];

		/* Link normal PINs with PUK */
		if (i < 2) {
			pin_obj.auth_id.len = 1;
			pin_obj.auth_id.value[0] = 3;
		}

		r = sc_pkcs15emu_add_pin_obj(p15card, &pin_obj, &pin_info);
		if (r < 0)
			return SC_ERROR_INTERNAL;
	}

	/* add private keys */
	for (i = 0; i < 2; i++) {
		static int prkey_pin[2] = {1, 2};

		static const char *prkey_name[2] = {
			"Isikutuvastus",
			"Allkirjastamine"};

		struct sc_pkcs15_prkey_info prkey_info;
		struct sc_pkcs15_object prkey_obj;

		memset(&prkey_info, 0, sizeof(prkey_info));
		memset(&prkey_obj, 0, sizeof(prkey_obj));

		prkey_info.id.len = 1;
		prkey_info.id.value[0] = prkey_pin[i];
		prkey_info.native = 1;
		prkey_info.key_reference = i + 1;
		prkey_info.field_length = field_length;
		prkey_info.modulus_length = modulus_length;
		if (i == 1)
			prkey_info.usage = SC_PKCS15_PRKEY_USAGE_NONREPUDIATION;
		else if(field_length > 0) // ECC has sign and derive usage
			prkey_info.usage = SC_PKCS15_PRKEY_USAGE_SIGN | SC_PKCS15_PRKEY_USAGE_DERIVE;
		else
			prkey_info.usage = SC_PKCS15_PRKEY_USAGE_SIGN | SC_PKCS15_PRKEY_USAGE_ENCRYPT | SC_PKCS15_PRKEY_USAGE_DECRYPT;

		strlcpy(prkey_obj.label, prkey_name[i], sizeof(prkey_obj.label));
		prkey_obj.auth_id.len = 1;
		prkey_obj.auth_id.value[0] = prkey_pin[i];
		prkey_obj.user_consent = 0;
		prkey_obj.flags = SC_PKCS15_CO_FLAG_PRIVATE;

		if(field_length > 0)
			r = sc_pkcs15emu_add_ec_prkey(p15card, &prkey_obj, &prkey_info);
		else
			r = sc_pkcs15emu_add_rsa_prkey(p15card, &prkey_obj, &prkey_info);
		if (r < 0)
			return SC_ERROR_INTERNAL;
	}

	return SC_SUCCESS;
}