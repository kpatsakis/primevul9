static xmlNodePtr to_xml_list(encodeTypePtr enc, zval *data, int style, xmlNodePtr parent TSRMLS_DC) {
	xmlNodePtr ret;
	encodePtr list_enc = NULL;

	if (enc->sdl_type && enc->sdl_type->kind == XSD_TYPEKIND_LIST && enc->sdl_type->elements) {
		sdlTypePtr *type;

		zend_hash_internal_pointer_reset(enc->sdl_type->elements);
		if (zend_hash_get_current_data(enc->sdl_type->elements, (void**)&type) == SUCCESS) {
			list_enc = (*type)->encode;
		}
	}

	ret = xmlNewNode(NULL, BAD_CAST("BOGUS"));
	xmlAddChild(parent, ret);
	FIND_ZVAL_NULL(data, ret, style);
	if (Z_TYPE_P(data) == IS_ARRAY) {
		zval **tmp;
		smart_str list = {0};
		HashTable *ht = Z_ARRVAL_P(data);

		zend_hash_internal_pointer_reset(ht);
		while (zend_hash_get_current_data(ht, (void**)&tmp) == SUCCESS) {
			xmlNodePtr dummy = master_to_xml(list_enc, *tmp, SOAP_LITERAL, ret TSRMLS_CC);
			if (dummy && dummy->children && dummy->children->content) {
				if (list.len != 0) {
					smart_str_appendc(&list, ' ');
				}
				smart_str_appends(&list, (char*)dummy->children->content);
			} else {
				soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
			}
			xmlUnlinkNode(dummy);
			xmlFreeNode(dummy);
			zend_hash_move_forward(ht);
		}
		smart_str_0(&list);
		xmlNodeSetContentLen(ret, BAD_CAST(list.c), list.len);
		smart_str_free(&list);
	} else {
		zval tmp = *data;
		char *str, *start, *next;
		smart_str list = {0};

		if (Z_TYPE_P(data) != IS_STRING) {
			zval_copy_ctor(&tmp);
			convert_to_string(&tmp);
			data = &tmp;
		}
		str = estrndup(Z_STRVAL_P(data), Z_STRLEN_P(data));
		whiteSpace_collapse(BAD_CAST(str));
		start = str;
		while (start != NULL && *start != '\0') {
			xmlNodePtr dummy;
			zval dummy_zval;

			next = strchr(start,' ');
			if (next != NULL) {
			  *next = '\0';
			  next++;
			}
			ZVAL_STRING(&dummy_zval, start, 0);
			dummy = master_to_xml(list_enc, &dummy_zval, SOAP_LITERAL, ret TSRMLS_CC);
			if (dummy && dummy->children && dummy->children->content) {
				if (list.len != 0) {
					smart_str_appendc(&list, ' ');
				}
				smart_str_appends(&list, (char*)dummy->children->content);
			} else {
				soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
			}
			xmlUnlinkNode(dummy);
			xmlFreeNode(dummy);

			start = next;
		}
		smart_str_0(&list);
		xmlNodeSetContentLen(ret, BAD_CAST(list.c), list.len);
		smart_str_free(&list);
		efree(str);
		if (data == &tmp) {zval_dtor(&tmp);}
	}
	return ret;
}