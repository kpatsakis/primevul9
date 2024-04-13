static xmlNodePtr to_xml_object(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	xmlNodePtr xmlParam;
	HashTable *prop = NULL;
	int i;
	sdlTypePtr sdlType = type->sdl_type;

	if (!data || Z_TYPE_P(data) == IS_NULL) {
		xmlParam = xmlNewNode(NULL, BAD_CAST("BOGUS"));
		xmlAddChild(parent, xmlParam);
		if (style == SOAP_ENCODED) {
			set_xsi_nil(xmlParam);
			set_ns_and_type(xmlParam, type);
		}
		return xmlParam;
	}

	if (Z_TYPE_P(data) == IS_OBJECT) {
		prop = Z_OBJPROP_P(data);
	} else if (Z_TYPE_P(data) == IS_ARRAY) {
		prop = Z_ARRVAL_P(data);
	}

	if (sdlType) {
		if (sdlType->kind == XSD_TYPEKIND_RESTRICTION &&
		    sdlType->encode && type != &sdlType->encode->details) {
			encodePtr enc;

			enc = sdlType->encode;
			while (enc && enc->details.sdl_type &&
			       enc->details.sdl_type->kind != XSD_TYPEKIND_SIMPLE &&
			       enc->details.sdl_type->kind != XSD_TYPEKIND_LIST &&
			       enc->details.sdl_type->kind != XSD_TYPEKIND_UNION) {
				enc = enc->details.sdl_type->encode;
			}
			if (enc) {
				zval *tmp = get_zval_property(data, "_" TSRMLS_CC);
				if (tmp) {
					xmlParam = master_to_xml(enc, tmp, style, parent TSRMLS_CC);
				} else if (prop == NULL) {
					xmlParam = master_to_xml(enc, data, style, parent TSRMLS_CC);
				} else {
					xmlParam = xmlNewNode(NULL, BAD_CAST("BOGUS"));
					xmlAddChild(parent, xmlParam);
				}
			} else {
				xmlParam = xmlNewNode(NULL, BAD_CAST("BOGUS"));
				xmlAddChild(parent, xmlParam);
			}
		} else if (sdlType->kind == XSD_TYPEKIND_EXTENSION &&
		           sdlType->encode && type != &sdlType->encode->details) {
			if (sdlType->encode->details.sdl_type &&
			    sdlType->encode->details.sdl_type->kind != XSD_TYPEKIND_SIMPLE &&
			    sdlType->encode->details.sdl_type->kind != XSD_TYPEKIND_LIST &&
			    sdlType->encode->details.sdl_type->kind != XSD_TYPEKIND_UNION) {

				if (prop) prop->nApplyCount++;
				xmlParam = master_to_xml(sdlType->encode, data, style, parent TSRMLS_CC);
				if (prop) prop->nApplyCount--;
			} else {
				zval *tmp = get_zval_property(data, "_" TSRMLS_CC);

				if (tmp) {
					xmlParam = master_to_xml(sdlType->encode, tmp, style, parent TSRMLS_CC);
				} else if (prop == NULL) {
					xmlParam = master_to_xml(sdlType->encode, data, style, parent TSRMLS_CC);
				} else {
					xmlParam = xmlNewNode(NULL, BAD_CAST("BOGUS"));
					xmlAddChild(parent, xmlParam);
				}
			}
		} else {
			xmlParam = xmlNewNode(NULL, BAD_CAST("BOGUS"));
			xmlAddChild(parent, xmlParam);
		}

		if (soap_check_zval_ref(data, xmlParam TSRMLS_CC)) {
			return xmlParam;
		}
		if (prop != NULL) {
			sdlTypePtr array_el;

			if (Z_TYPE_P(data) == IS_ARRAY &&
		      !is_map(data) &&
		      sdlType->attributes == NULL &&
		      sdlType->model != NULL &&
		      (array_el = model_array_element(sdlType->model)) != NULL) {
				zval **val;

				zend_hash_internal_pointer_reset(prop);
				while (zend_hash_get_current_data(prop,(void**)&val) == SUCCESS) {
					xmlNodePtr property;
					if (Z_TYPE_PP(val) == IS_NULL && array_el->nillable) {
						property = xmlNewNode(NULL, BAD_CAST("BOGUS"));
						xmlAddChild(xmlParam, property);
						set_xsi_nil(property);
					} else {
						property = master_to_xml(array_el->encode, *val, style, xmlParam TSRMLS_CC);
					}
					xmlNodeSetName(property, BAD_CAST(array_el->name));
					if (style == SOAP_LITERAL &&
					   array_el->namens &&
					   array_el->form == XSD_FORM_QUALIFIED) {
						xmlNsPtr nsp = encode_add_ns(property, array_el->namens);
						xmlSetNs(property, nsp);
					}
					zend_hash_move_forward(prop);
				}
			} else if (sdlType->model) {
				model_to_xml_object(xmlParam, sdlType->model, data, style, 1 TSRMLS_CC);
			}
			if (sdlType->attributes) {
				sdlAttributePtr *attr;
				zval *zattr;
				HashPosition pos;

				zend_hash_internal_pointer_reset_ex(sdlType->attributes, &pos);
				while (zend_hash_get_current_data_ex(sdlType->attributes, (void**)&attr, &pos) == SUCCESS) {
					if ((*attr)->name) {
						zattr = get_zval_property(data, (*attr)->name TSRMLS_CC);
						if (zattr) {
							xmlNodePtr dummy;

							dummy = master_to_xml((*attr)->encode, zattr, SOAP_LITERAL, xmlParam TSRMLS_CC);
							if (dummy->children && dummy->children->content) {
								if ((*attr)->fixed && strcmp((*attr)->fixed, (char*)dummy->children->content) != 0) {
									soap_error3(E_ERROR, "Encoding: Attribute '%s' has fixed value '%s' (value '%s' is not allowed)", (*attr)->name, (*attr)->fixed, dummy->children->content);
								}
								/* we need to handle xml: namespace specially, since it is
								   an implicit schema. Otherwise, use form.
								*/
								if ((*attr)->namens &&
								    (!strncmp((*attr)->namens, XML_NAMESPACE, sizeof(XML_NAMESPACE)) ||
								     (*attr)->form == XSD_FORM_QUALIFIED)) {
									xmlNsPtr nsp = encode_add_ns(xmlParam, (*attr)->namens);

									xmlSetNsProp(xmlParam, nsp, BAD_CAST((*attr)->name), dummy->children->content);
								} else {
									xmlSetProp(xmlParam, BAD_CAST((*attr)->name), dummy->children->content);
								}
							}
							xmlUnlinkNode(dummy);
							xmlFreeNode(dummy);
						}
					}
					zend_hash_move_forward_ex(sdlType->attributes, &pos);
				}
			}
		}
		if (style == SOAP_ENCODED) {
			set_ns_and_type(xmlParam, type);
		}
	} else {
		xmlParam = xmlNewNode(NULL, BAD_CAST("BOGUS"));
		xmlAddChild(parent, xmlParam);

		if (soap_check_zval_ref(data, xmlParam TSRMLS_CC)) {
			return xmlParam;
		}
		if (prop != NULL) {
			i = zend_hash_num_elements(prop);
			zend_hash_internal_pointer_reset(prop);

			for (;i > 0;i--) {
				xmlNodePtr property;
				zval **zprop;
				char *str_key;
				ulong index;
				int key_type;
				unsigned int str_key_len;

				key_type = zend_hash_get_current_key_ex(prop, &str_key, &str_key_len, &index, FALSE, NULL);
				zend_hash_get_current_data(prop, (void **)&zprop);

				property = master_to_xml(get_conversion((*zprop)->type), (*zprop), style, xmlParam TSRMLS_CC);

				if (key_type == HASH_KEY_IS_STRING) {
					const char *prop_name;

					if (Z_TYPE_P(data) == IS_OBJECT) {
						const char *class_name;

						zend_unmangle_property_name(str_key, str_key_len-1, &class_name, &prop_name);
					} else {
						prop_name = str_key;
					}
					if (prop_name) {
						xmlNodeSetName(property, BAD_CAST(prop_name));
					}
				}
				zend_hash_move_forward(prop);
			}
		}
		if (style == SOAP_ENCODED) {
			set_ns_and_type(xmlParam, type);
		}
	}
	return xmlParam;
}