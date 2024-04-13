 */
void _php_imap_add_body(zval *arg, BODY *body)
{
	zval parametres, param, dparametres, dparam;
	PARAMETER *par, *dpar;
	PART *part;

	if (body->type <= TYPEMAX) {
		add_property_long(arg, "type", body->type);
	}

	if (body->encoding <= ENCMAX) {
		add_property_long(arg, "encoding", body->encoding);
	}

	if (body->subtype) {
		add_property_long(arg, "ifsubtype", 1);
		add_property_string(arg, "subtype", body->subtype);
	} else {
		add_property_long(arg, "ifsubtype", 0);
	}

	if (body->description) {
		add_property_long(arg, "ifdescription", 1);
		add_property_string(arg, "description", body->description);
	} else {
		add_property_long(arg, "ifdescription", 0);
	}

	if (body->id) {
		add_property_long(arg, "ifid", 1);
		add_property_string(arg, "id", body->id);
	} else {
		add_property_long(arg, "ifid", 0);
	}

	if (body->size.lines) {
		add_property_long(arg, "lines", body->size.lines);
	}

	if (body->size.bytes) {
		add_property_long(arg, "bytes", body->size.bytes);
	}

#ifdef IMAP41
	if (body->disposition.type) {
		add_property_long(arg, "ifdisposition", 1);
		add_property_string(arg, "disposition", body->disposition.type);
	} else {
		add_property_long(arg, "ifdisposition", 0);
	}

	if (body->disposition.parameter) {
		dpar = body->disposition.parameter;
		add_property_long(arg, "ifdparameters", 1);
		array_init(&dparametres);
		do {
			object_init(&dparam);
			add_property_string(&dparam, "attribute", dpar->attribute);
			add_property_string(&dparam, "value", dpar->value);
			add_next_index_object(&dparametres, &dparam);
		} while ((dpar = dpar->next));
		add_assoc_object(arg, "dparameters", &dparametres);
	} else {
		add_property_long(arg, "ifdparameters", 0);
	}
#endif

	if ((par = body->parameter)) {
		add_property_long(arg, "ifparameters", 1);

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
		add_property_long(arg, "ifparameters", 0);
	}
	add_assoc_object(arg, "parameters", &parametres);

	/* multipart message ? */
	if (body->type == TYPEMULTIPART) {
		array_init(&parametres);
		for (part = body->CONTENT_PART; part; part = part->next) {
			object_init(&param);
			_php_imap_add_body(&param, &part->body);
			add_next_index_object(&parametres, &param);
		}
		add_assoc_object(arg, "parts", &parametres);
	}

	/* encapsulated message ? */
	if ((body->type == TYPEMESSAGE) && (!strcasecmp(body->subtype, "rfc822"))) {
		body = body->CONTENT_MSG_BODY;
		array_init(&parametres);
		object_init(&param);
		_php_imap_add_body(&param, body);
		add_next_index_object(&parametres, &param);
		add_assoc_object(arg, "parts", &parametres);
	}