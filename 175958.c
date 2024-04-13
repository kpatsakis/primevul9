static zval *guess_zval_convert(encodeTypePtr type, xmlNodePtr data TSRMLS_DC)
{
	encodePtr enc = NULL;
	xmlAttrPtr tmpattr;
	xmlChar *type_name = NULL;
	zval *ret;

	data = check_and_resolve_href(data);

	if (data == NULL) {
		enc = get_conversion(IS_NULL);
	} else if (data->properties && get_attribute_ex(data->properties, "nil", XSI_NAMESPACE)) {
		enc = get_conversion(IS_NULL);
	} else {
		tmpattr = get_attribute_ex(data->properties,"type", XSI_NAMESPACE);
		if (tmpattr != NULL) {
		  type_name = tmpattr->children->content;
			enc = get_encoder_from_prefix(SOAP_GLOBAL(sdl), data, tmpattr->children->content);
			if (enc && type == &enc->details) {
				enc = NULL;
			}
			if (enc != NULL) {
			  encodePtr tmp = enc;
			  while (tmp &&
			         tmp->details.sdl_type != NULL &&
			         tmp->details.sdl_type->kind != XSD_TYPEKIND_COMPLEX) {
			    if (enc == tmp->details.sdl_type->encode ||
			        tmp == tmp->details.sdl_type->encode) {
			    	enc = NULL;
			    	break;
			    }
			    tmp = tmp->details.sdl_type->encode;
			  }
			}
		}

		if (enc == NULL) {
			/* Didn't have a type, totally guess here */
			/* Logic: has children = IS_OBJECT else IS_STRING */
			xmlNodePtr trav;

			if (get_attribute(data->properties, "arrayType") ||
			    get_attribute(data->properties, "itemType") ||
			    get_attribute(data->properties, "arraySize")) {
				enc = get_conversion(SOAP_ENC_ARRAY);
			} else {
				enc = get_conversion(XSD_STRING);
				trav = data->children;
				while (trav != NULL) {
					if (trav->type == XML_ELEMENT_NODE) {
						enc = get_conversion(SOAP_ENC_OBJECT);
						break;
					}
					trav = trav->next;
				}
			}
		}
	}
	ret = master_to_zval_int(enc, data TSRMLS_CC);
	if (SOAP_GLOBAL(sdl) && type_name && enc->details.sdl_type) {
		zval* soapvar;
		char *ns, *cptype;
		xmlNsPtr nsptr;

		MAKE_STD_ZVAL(soapvar);
		object_init_ex(soapvar, soap_var_class_entry);
		add_property_long(soapvar, "enc_type", enc->details.type);
		Z_DELREF_P(ret);
		add_property_zval(soapvar, "enc_value", ret);
		parse_namespace(type_name, &cptype, &ns);
		nsptr = xmlSearchNs(data->doc, data, BAD_CAST(ns));
		add_property_string(soapvar, "enc_stype", cptype, 1);
		if (nsptr) {
			add_property_string(soapvar, "enc_ns", (char*)nsptr->href, 1);
		}
		efree(cptype);
		if (ns) {efree(ns);}
		ret = soapvar;
	}
	return ret;
}