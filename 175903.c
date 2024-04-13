static xmlNodePtr to_xml_array(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	sdlTypePtr sdl_type = type->sdl_type;
	sdlTypePtr element_type = NULL;
	smart_str array_type = {0}, array_size = {0};
	int i;
	xmlNodePtr xmlParam;
	encodePtr enc = NULL;
	int dimension = 1;
	int* dims;
	int soap_version;
	zval *array_copy = NULL;

	soap_version = SOAP_GLOBAL(soap_version);

	xmlParam = xmlNewNode(NULL, BAD_CAST("BOGUS"));
	xmlAddChild(parent, xmlParam);

	if (!data || Z_TYPE_P(data) == IS_NULL) {
		if (style == SOAP_ENCODED) {
			set_xsi_nil(xmlParam);
			if (SOAP_GLOBAL(features) & SOAP_USE_XSI_ARRAY_TYPE) {
				set_ns_and_type_ex(xmlParam, (soap_version == SOAP_1_1) ? SOAP_1_1_ENC_NAMESPACE : SOAP_1_2_ENC_NAMESPACE, "Array");
			} else {
				set_ns_and_type(xmlParam, type);
			}
		}
		return xmlParam;
	}

	if (Z_TYPE_P(data) == IS_OBJECT && instanceof_function(Z_OBJCE_P(data), zend_ce_traversable TSRMLS_CC)) {
		zend_object_iterator   *iter;
		zend_class_entry       *ce = Z_OBJCE_P(data);
		zval                  **val;
		char                   *str_key;
		uint                    str_key_len;
		ulong                   int_key;
		int                     key_type;

		ALLOC_ZVAL(array_copy);
		INIT_PZVAL(array_copy);
		array_init(array_copy);

		iter = ce->get_iterator(ce, data, 0 TSRMLS_CC);

		if (EG(exception)) {
			goto iterator_done;
		}

		if (iter->funcs->rewind) {
			iter->funcs->rewind(iter TSRMLS_CC);
			if (EG(exception)) {
				goto iterator_done;
			}
		}

		while (iter->funcs->valid(iter TSRMLS_CC) == SUCCESS) {
			if (EG(exception)) {
				goto iterator_done;
			}

			iter->funcs->get_current_data(iter, &val TSRMLS_CC);
			if (EG(exception)) {
				goto iterator_done;
			}
			if (iter->funcs->get_current_key) {
				key_type = iter->funcs->get_current_key(iter, &str_key, &str_key_len, &int_key TSRMLS_CC);
				if (EG(exception)) {
					goto iterator_done;
				}
				switch(key_type) {
					case HASH_KEY_IS_STRING:
						add_assoc_zval_ex(array_copy, str_key, str_key_len, *val);
						efree(str_key);
						break;
					case HASH_KEY_IS_LONG:
						add_index_zval(array_copy, int_key, *val);
						break;
				}
			} else {
				add_next_index_zval(array_copy, *val);
			}
			Z_ADDREF_PP(val);

			iter->funcs->move_forward(iter TSRMLS_CC);
			if (EG(exception)) {
				goto iterator_done;
			}
		}
iterator_done:
		iter->funcs->dtor(iter TSRMLS_CC);
		if (EG(exception)) {
			zval_ptr_dtor(&array_copy);
			array_copy = NULL;
		} else {
			data = array_copy;
		}
	}

	if (Z_TYPE_P(data) == IS_ARRAY) {
		sdlAttributePtr *arrayType;
		sdlExtraAttributePtr *ext;
		sdlTypePtr elementType;

		i = zend_hash_num_elements(Z_ARRVAL_P(data));

		if (sdl_type &&
		    sdl_type->attributes &&
		    zend_hash_find(sdl_type->attributes, SOAP_1_1_ENC_NAMESPACE":arrayType",
		      sizeof(SOAP_1_1_ENC_NAMESPACE":arrayType"),
		      (void **)&arrayType) == SUCCESS &&
		    (*arrayType)->extraAttributes &&
		    zend_hash_find((*arrayType)->extraAttributes, WSDL_NAMESPACE":arrayType", sizeof(WSDL_NAMESPACE":arrayType"), (void **)&ext) == SUCCESS) {

			char *value, *end;
			zval** el;

			value = estrdup((*ext)->val);
			end = strrchr(value,'[');
			if (end) {
				*end = '\0';
				end++;
				dimension = calc_dimension(end);
			}
			if ((*ext)->ns != NULL) {
				enc = get_encoder(SOAP_GLOBAL(sdl), (*ext)->ns, value);
				get_type_str(xmlParam, (*ext)->ns, value, &array_type);
			} else {
				smart_str_appends(&array_type, value);
			}

			dims = safe_emalloc(sizeof(int), dimension, 0);
			dims[0] = i;
			el = &data;
			for (i = 1; i < dimension; i++) {
				if (el != NULL && Z_TYPE_PP(el) == IS_ARRAY &&
				    zend_hash_num_elements(Z_ARRVAL_PP(el)) > 0) {
				  zend_hash_internal_pointer_reset(Z_ARRVAL_PP(el));
					zend_hash_get_current_data(Z_ARRVAL_PP(el), (void**)&el);
					if (Z_TYPE_PP(el) == IS_ARRAY) {
						dims[i] = zend_hash_num_elements(Z_ARRVAL_PP(el));
					} else {
						dims[i] = 0;
					}
				}
			}

			smart_str_append_long(&array_size, dims[0]);
			for (i=1; i<dimension; i++) {
				smart_str_appendc(&array_size, ',');
				smart_str_append_long(&array_size, dims[i]);
			}

			efree(value);

		} else if (sdl_type &&
		           sdl_type->attributes &&
		           zend_hash_find(sdl_type->attributes, SOAP_1_2_ENC_NAMESPACE":itemType",
		             sizeof(SOAP_1_2_ENC_NAMESPACE":itemType"),
		             (void **)&arrayType) == SUCCESS &&
		           (*arrayType)->extraAttributes &&
		           zend_hash_find((*arrayType)->extraAttributes, WSDL_NAMESPACE":itemType", sizeof(WSDL_NAMESPACE":itemType"), (void **)&ext) == SUCCESS) {
			if ((*ext)->ns != NULL) {
				enc = get_encoder(SOAP_GLOBAL(sdl), (*ext)->ns, (*ext)->val);
				get_type_str(xmlParam, (*ext)->ns, (*ext)->val, &array_type);
			} else {
				smart_str_appends(&array_type, (*ext)->val);
			}
			if (zend_hash_find(sdl_type->attributes, SOAP_1_2_ENC_NAMESPACE":arraySize",
			                   sizeof(SOAP_1_2_ENC_NAMESPACE":arraySize"),
			                   (void **)&arrayType) == SUCCESS &&
			    (*arrayType)->extraAttributes &&
			    zend_hash_find((*arrayType)->extraAttributes, WSDL_NAMESPACE":arraySize", sizeof(WSDL_NAMESPACE":arraysize"), (void **)&ext) == SUCCESS) {
				dimension = calc_dimension_12((*ext)->val);
				dims = get_position_12(dimension, (*ext)->val);
				if (dims[0] == 0) {dims[0] = i;}

				smart_str_append_long(&array_size, dims[0]);
				for (i=1; i<dimension; i++) {
					smart_str_appendc(&array_size, ',');
					smart_str_append_long(&array_size, dims[i]);
				}
			} else {
				dims = emalloc(sizeof(int));
				*dims = 0;
				smart_str_append_long(&array_size, i);
			}
		} else if (sdl_type &&
		           sdl_type->attributes &&
		           zend_hash_find(sdl_type->attributes, SOAP_1_2_ENC_NAMESPACE":arraySize",
		             sizeof(SOAP_1_2_ENC_NAMESPACE":arraySize"),
		             (void **)&arrayType) == SUCCESS &&
		           (*arrayType)->extraAttributes &&
		           zend_hash_find((*arrayType)->extraAttributes, WSDL_NAMESPACE":arraySize", sizeof(WSDL_NAMESPACE":arraySize"), (void **)&ext) == SUCCESS) {
			dimension = calc_dimension_12((*ext)->val);
			dims = get_position_12(dimension, (*ext)->val);
			if (dims[0] == 0) {dims[0] = i;}

			smart_str_append_long(&array_size, dims[0]);
			for (i=1; i<dimension; i++) {
				smart_str_appendc(&array_size, ',');
				smart_str_append_long(&array_size, dims[i]);
			}

			if (sdl_type && sdl_type->elements &&
			    zend_hash_num_elements(sdl_type->elements) == 1 &&
			    (zend_hash_internal_pointer_reset(sdl_type->elements),
			     zend_hash_get_current_data(sdl_type->elements, (void**)&elementType) == SUCCESS) &&
					(elementType = *(sdlTypePtr*)elementType) != NULL &&
			     elementType->encode && elementType->encode->details.type_str) {
				element_type = elementType;
				enc = elementType->encode;
				get_type_str(xmlParam, elementType->encode->details.ns, elementType->encode->details.type_str, &array_type);
			} else {
				enc = get_array_type(xmlParam, data, &array_type TSRMLS_CC);
			}
		} else if (sdl_type && sdl_type->elements &&
		           zend_hash_num_elements(sdl_type->elements) == 1 &&
		           (zend_hash_internal_pointer_reset(sdl_type->elements),
		            zend_hash_get_current_data(sdl_type->elements, (void**)&elementType) == SUCCESS) &&
		           (elementType = *(sdlTypePtr*)elementType) != NULL &&
		           elementType->encode && elementType->encode->details.type_str) {

			element_type = elementType;
			enc = elementType->encode;
			get_type_str(xmlParam, elementType->encode->details.ns, elementType->encode->details.type_str, &array_type);

			smart_str_append_long(&array_size, i);

			dims = safe_emalloc(sizeof(int), dimension, 0);
			dims[0] = i;
		} else {

			enc = get_array_type(xmlParam, data, &array_type TSRMLS_CC);
			smart_str_append_long(&array_size, i);
			dims = safe_emalloc(sizeof(int), dimension, 0);
			dims[0] = i;
		}

		if (style == SOAP_ENCODED) {
			if (soap_version == SOAP_1_1) {
				smart_str_0(&array_type);
				if (strcmp(array_type.c,"xsd:anyType") == 0) {
					smart_str_free(&array_type);
					smart_str_appendl(&array_type,"xsd:ur-type",sizeof("xsd:ur-type")-1);
				}
				smart_str_appendc(&array_type, '[');
				smart_str_append(&array_type, &array_size);
				smart_str_appendc(&array_type, ']');
				smart_str_0(&array_type);
				set_ns_prop(xmlParam, SOAP_1_1_ENC_NAMESPACE, "arrayType", array_type.c);
			} else {
				int i = 0;
				while (i < array_size.len) {
					if (array_size.c[i] == ',') {array_size.c[i] = ' ';}
					++i;
				}
				smart_str_0(&array_type);
				smart_str_0(&array_size);
				set_ns_prop(xmlParam, SOAP_1_2_ENC_NAMESPACE, "itemType", array_type.c);
				set_ns_prop(xmlParam, SOAP_1_2_ENC_NAMESPACE, "arraySize", array_size.c);
			}
		}
		smart_str_free(&array_type);
		smart_str_free(&array_size);

		add_xml_array_elements(xmlParam, element_type, enc, enc?encode_add_ns(xmlParam,enc->details.ns):NULL, dimension, dims, data, style TSRMLS_CC);
		efree(dims);
	}
	if (style == SOAP_ENCODED) {
		if (SOAP_GLOBAL(features) & SOAP_USE_XSI_ARRAY_TYPE) {
			set_ns_and_type_ex(xmlParam, (soap_version == SOAP_1_1) ? SOAP_1_1_ENC_NAMESPACE : SOAP_1_2_ENC_NAMESPACE, "Array");
		} else {
			set_ns_and_type(xmlParam, type);
		}
	}

	if (array_copy) {
		zval_ptr_dtor(&array_copy);
	}

	return xmlParam;
}