   Read the structure of a specified body section of a specific message */
PHP_FUNCTION(imap_bodystruct)
{
	zval *streamind;
	zend_long msg;
	zend_string *section;
	pils *imap_le_struct;
	zval parametres, param, dparametres, dparam;
	PARAMETER *par, *dpar;
	BODY *body;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rlS", &streamind, &msg, &section) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if (!msg || msg < 1 || (unsigned) msg > imap_le_struct->imap_stream->nmsgs) {
		php_error_docref(NULL, E_WARNING, "Bad message number");
		RETURN_FALSE;
	}

	object_init(return_value);

	body=mail_body(imap_le_struct->imap_stream, msg, (unsigned char*)ZSTR_VAL(section));
	if (body == NULL) {
		zval_dtor(return_value);
		RETURN_FALSE;
	}
	if (body->type <= TYPEMAX) {
		add_property_long(return_value, "type", body->type);
	}
	if (body->encoding <= ENCMAX) {
		add_property_long(return_value, "encoding", body->encoding);
	}

	if (body->subtype) {
		add_property_long(return_value, "ifsubtype", 1);
		add_property_string(return_value, "subtype", body->subtype);
	} else {
		add_property_long(return_value, "ifsubtype", 0);
	}

	if (body->description) {
		add_property_long(return_value, "ifdescription", 1);
		add_property_string(return_value, "description", body->description);
	} else {
		add_property_long(return_value, "ifdescription", 0);
	}
	if (body->id) {
		add_property_long(return_value, "ifid", 1);
		add_property_string(return_value, "id", body->id);
	} else {
		add_property_long(return_value, "ifid", 0);
	}

	if (body->size.lines) {
		add_property_long(return_value, "lines", body->size.lines);
	}
	if (body->size.bytes) {
		add_property_long(return_value, "bytes", body->size.bytes);
	}
#ifdef IMAP41
	if (body->disposition.type) {
		add_property_long(return_value, "ifdisposition", 1);
		add_property_string(return_value, "disposition", body->disposition.type);
	} else {
		add_property_long(return_value, "ifdisposition", 0);
	}

	if (body->disposition.parameter) {
		dpar = body->disposition.parameter;
		add_property_long(return_value, "ifdparameters", 1);
		array_init(&dparametres);
		do {
			object_init(&dparam);
			add_property_string(&dparam, "attribute", dpar->attribute);
			add_property_string(&dparam, "value", dpar->value);
			add_next_index_object(&dparametres, &dparam);
		} while ((dpar = dpar->next));
		add_assoc_object(return_value, "dparameters", &dparametres);
	} else {
		add_property_long(return_value, "ifdparameters", 0);
	}
#endif

	if ((par = body->parameter)) {
		add_property_long(return_value, "ifparameters", 1);

		array_init(&parametres);
		do {
			object_init(&param);
			if (par->attribute) {
				add_property_string(&param, "attribute", par->attribute);
			}
			if (par->value) {
				add_property_string(&param, "value", par->value);
			}

			add_next_index_object(&parametres, &param);
		} while ((par = par->next));
	} else {
		object_init(&parametres);
		add_property_long(return_value, "ifparameters", 0);
	}
	add_assoc_object(return_value, "parameters", &parametres);