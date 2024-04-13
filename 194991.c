sc_pkcs15emu_oberthur_init(struct sc_pkcs15_card * p15card)
{
	struct sc_context *ctx = p15card->card->ctx;
	struct sc_pkcs15_auth_info auth_info;
	struct sc_pkcs15_object   obj;
	struct sc_card *card = p15card->card;
	struct sc_path path;
	int rv, ii, tries_left;
	char serial[0x10];
	unsigned char sopin_reference = 0x04;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);
	sc_bin_to_hex(card->serialnr.value, card->serialnr.len, serial, sizeof(serial), 0);
	set_string(&p15card->tokeninfo->serial_number, serial);

	p15card->ops.parse_df = sc_awp_parse_df;
	p15card->ops.clear = sc_awp_clear;

	sc_log(ctx, "Oberthur init: serial %s", p15card->tokeninfo->serial_number);

	sc_format_path(AWP_PIN_DF, &path);
	rv = sc_select_file(card, &path, NULL);
	LOG_TEST_RET(ctx, rv, "Oberthur init failed: cannot select PIN dir");

	tries_left = -1;
	rv = sc_verify(card, SC_AC_CHV, sopin_reference, (unsigned char *)"", 0, &tries_left);
	if (rv && rv != SC_ERROR_PIN_CODE_INCORRECT)   {
		sopin_reference = 0x84;
		rv = sc_verify(card, SC_AC_CHV, sopin_reference, (unsigned char *)"", 0, &tries_left);
	}
	if (rv && rv != SC_ERROR_PIN_CODE_INCORRECT)
		LOG_TEST_RET(ctx, rv, "Invalid state of SO-PIN");

	/* add PIN */
	memset(&auth_info, 0, sizeof(auth_info));
	memset(&obj,  0, sizeof(obj));

	auth_info.auth_type = SC_PKCS15_PIN_AUTH_TYPE_PIN;
	auth_info.auth_method	= SC_AC_CHV;
	auth_info.auth_id.len = 1;
	auth_info.auth_id.value[0] = 0xFF;
	auth_info.attrs.pin.min_length		= 4;
	auth_info.attrs.pin.max_length		= 64;
	auth_info.attrs.pin.stored_length	= 64;
	auth_info.attrs.pin.type		= SC_PKCS15_PIN_TYPE_ASCII_NUMERIC;
	auth_info.attrs.pin.reference		= sopin_reference;
	auth_info.attrs.pin.pad_char		= 0xFF;
	auth_info.attrs.pin.flags		= SC_PKCS15_PIN_FLAG_CASE_SENSITIVE
				| SC_PKCS15_PIN_FLAG_INITIALIZED
				| SC_PKCS15_PIN_FLAG_NEEDS_PADDING
				| SC_PKCS15_PIN_FLAG_SO_PIN;
	auth_info.tries_left		= tries_left;
	auth_info.logged_in = SC_PIN_STATE_UNKNOWN;

	strncpy(obj.label, "SO PIN", SC_PKCS15_MAX_LABEL_SIZE-1);
	obj.flags = SC_PKCS15_CO_FLAG_MODIFIABLE | SC_PKCS15_CO_FLAG_PRIVATE;

	sc_log(ctx, "Add PIN(%s,auth_id:%s,reference:%i)", obj.label,
			sc_pkcs15_print_id(&auth_info.auth_id), auth_info.attrs.pin.reference);
	rv = sc_pkcs15emu_add_pin_obj(p15card, &obj, &auth_info);
	LOG_TEST_RET(ctx, rv, "Oberthur init failed: cannot add PIN object");

	tries_left = -1;
	rv = sc_verify(card, SC_AC_CHV, 0x81, (unsigned char *)"", 0, &tries_left);
	if (rv == SC_ERROR_PIN_CODE_INCORRECT)   {
		/* add PIN */
		memset(&auth_info, 0, sizeof(auth_info));
		memset(&obj,  0, sizeof(obj));

		auth_info.auth_id.len = sizeof(PinDomainID) > sizeof(auth_info.auth_id.value)
				? sizeof(auth_info.auth_id.value) : sizeof(PinDomainID);
		memcpy(auth_info.auth_id.value, PinDomainID, auth_info.auth_id.len);
		auth_info.auth_method	= SC_AC_CHV;

		auth_info.attrs.pin.min_length		= 4;
		auth_info.attrs.pin.max_length		= 64;
		auth_info.attrs.pin.stored_length	= 64;
		auth_info.attrs.pin.type		= SC_PKCS15_PIN_TYPE_ASCII_NUMERIC;
		auth_info.attrs.pin.reference		= 0x81;
		auth_info.attrs.pin.pad_char		= 0xFF;
		auth_info.attrs.pin.flags		= SC_PKCS15_PIN_FLAG_CASE_SENSITIVE
					| SC_PKCS15_PIN_FLAG_INITIALIZED
					| SC_PKCS15_PIN_FLAG_NEEDS_PADDING
					| SC_PKCS15_PIN_FLAG_LOCAL;
		auth_info.tries_left		= tries_left;

		strncpy(obj.label, PIN_DOMAIN_LABEL, SC_PKCS15_MAX_LABEL_SIZE-1);
		obj.flags = SC_PKCS15_CO_FLAG_MODIFIABLE | SC_PKCS15_CO_FLAG_PRIVATE;
		if (sopin_reference == 0x84) {
			/*
			 * auth_pin_reset_oberthur_style() in card-oberthur.c
			 * always uses PUK with reference 0x84 for
			 * unblocking of User PIN
			 */
			obj.auth_id.len = 1;
			obj.auth_id.value[0] = 0xFF;
		}

		sc_format_path(AWP_PIN_DF, &auth_info.path);
		auth_info.path.type = SC_PATH_TYPE_PATH;

		sc_log(ctx, "Add PIN(%s,auth_id:%s,reference:%i)", obj.label,
				sc_pkcs15_print_id(&auth_info.auth_id), auth_info.attrs.pin.reference);
		rv = sc_pkcs15emu_add_pin_obj(p15card, &obj, &auth_info);
		LOG_TEST_RET(ctx, rv, "Oberthur init failed: cannot add PIN object");
	}
	else if (rv != SC_ERROR_DATA_OBJECT_NOT_FOUND)    {
		LOG_TEST_RET(ctx, rv, "Oberthur init failed: cannot verify PIN");
	}

	for (ii=0; oberthur_infos[ii].name; ii++)   {
		sc_log(ctx, "Oberthur init: read %s file", oberthur_infos[ii].name);
		rv = sc_oberthur_read_file(p15card, oberthur_infos[ii].path,
				&oberthur_infos[ii].content, &oberthur_infos[ii].len, 1);
		LOG_TEST_RET(ctx, rv, "Oberthur init failed: read oberthur file error");

		sc_log(ctx,
		       "Oberthur init: parse %s file, content length %"SC_FORMAT_LEN_SIZE_T"u",
		       oberthur_infos[ii].name, oberthur_infos[ii].len);
		rv = oberthur_infos[ii].parser(p15card, oberthur_infos[ii].content, oberthur_infos[ii].len,
				oberthur_infos[ii].postpone_allowed);
		LOG_TEST_RET(ctx, rv, "Oberthur init failed: parse error");
	}

	LOG_FUNC_RETURN(ctx, SC_SUCCESS);
}