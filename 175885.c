static zval *to_zval_object_ex(encodeTypePtr type, xmlNodePtr data, zend_class_entry *pce TSRMLS_DC)
{
	zval *ret;
	xmlNodePtr trav;
	sdlPtr sdl;
	sdlTypePtr sdlType = type->sdl_type;
	zend_class_entry *ce = ZEND_STANDARD_CLASS_DEF_PTR;
	zval *redo_any = NULL;

	if (pce) {
		ce = pce;
	} else if (SOAP_GLOBAL(class_map) && type->type_str) {
		zval             **classname;
		zend_class_entry  *tmp;

		if (zend_hash_find(SOAP_GLOBAL(class_map), type->type_str, strlen(type->type_str)+1, (void**)&classname) == SUCCESS &&
		    Z_TYPE_PP(classname) == IS_STRING &&
		    (tmp = zend_fetch_class(Z_STRVAL_PP(classname), Z_STRLEN_PP(classname), ZEND_FETCH_CLASS_AUTO TSRMLS_CC)) != NULL) {
			ce = tmp;
		}
	}
	sdl = SOAP_GLOBAL(sdl);
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
				zval *base;

				ALLOC_INIT_ZVAL(ret);
				if (soap_check_xml_ref(&ret, data TSRMLS_CC)) {
					return ret;
				}

				object_init_ex(ret, ce);
				base = master_to_zval_int(enc, data TSRMLS_CC);
				set_zval_property(ret, "_", base TSRMLS_CC);
			} else {
				ALLOC_INIT_ZVAL(ret);
				FIND_XML_NULL(data, ret);
				if (soap_check_xml_ref(&ret, data TSRMLS_CC)) {
					return ret;
				}
				object_init_ex(ret, ce);
			}
		} else if (sdlType->kind == XSD_TYPEKIND_EXTENSION &&
		           sdlType->encode &&
		           type != &sdlType->encode->details) {
			if (sdlType->encode->details.sdl_type &&
			    sdlType->encode->details.sdl_type->kind != XSD_TYPEKIND_SIMPLE &&
			    sdlType->encode->details.sdl_type->kind != XSD_TYPEKIND_LIST &&
			    sdlType->encode->details.sdl_type->kind != XSD_TYPEKIND_UNION) {

				CHECK_XML_NULL(data);
				if ((ret = soap_find_xml_ref(data TSRMLS_CC)) != NULL) {
					return ret;
				}

			    if (ce != ZEND_STANDARD_CLASS_DEF_PTR &&
			        sdlType->encode->to_zval == sdl_guess_convert_zval &&
			        sdlType->encode->details.sdl_type != NULL &&
			        (sdlType->encode->details.sdl_type->kind == XSD_TYPEKIND_COMPLEX ||
			         sdlType->encode->details.sdl_type->kind == XSD_TYPEKIND_RESTRICTION ||
			         sdlType->encode->details.sdl_type->kind == XSD_TYPEKIND_EXTENSION) &&
			        (sdlType->encode->details.sdl_type->encode == NULL ||
			         (sdlType->encode->details.sdl_type->encode->details.type != IS_ARRAY &&
			          sdlType->encode->details.sdl_type->encode->details.type != SOAP_ENC_ARRAY))) {
					ret = to_zval_object_ex(&sdlType->encode->details, data, ce TSRMLS_CC);
			    } else {
					ret = master_to_zval_int(sdlType->encode, data TSRMLS_CC);
				}
				if (soap_check_xml_ref(&ret, data TSRMLS_CC)) {
					return ret;
				}
				redo_any = get_zval_property(ret, "any" TSRMLS_CC);
				if (Z_TYPE_P(ret) == IS_OBJECT && ce != ZEND_STANDARD_CLASS_DEF_PTR) {
					zend_object *zobj = zend_objects_get_address(ret TSRMLS_CC);
					zobj->ce = ce;
				}
			} else {
				zval *base;

				ALLOC_INIT_ZVAL(ret);
				if (soap_check_xml_ref(&ret, data TSRMLS_CC)) {
					return ret;
				}

				object_init_ex(ret, ce);
				base = master_to_zval_int(sdlType->encode, data TSRMLS_CC);
				set_zval_property(ret, "_", base TSRMLS_CC);
			}
		} else {
			ALLOC_INIT_ZVAL(ret);
			FIND_XML_NULL(data, ret);
			if (soap_check_xml_ref(&ret, data TSRMLS_CC)) {
				return ret;
			}
			object_init_ex(ret, ce);
		}
		if (sdlType->model) {
			if (redo_any) {
				Z_ADDREF_P(redo_any);
				unset_zval_property(ret, "any" TSRMLS_CC);
			}
			model_to_zval_object(ret, sdlType->model, data, sdl TSRMLS_CC);
			if (redo_any) {
				zval *tmp = get_zval_property(ret, "any" TSRMLS_CC);

				if (tmp == NULL) {
					model_to_zval_any(ret, data->children TSRMLS_CC);
				} else if (Z_REFCOUNT_P(tmp) == 0) {
					zval_dtor(tmp);
					efree(tmp);
				}
				zval_ptr_dtor(&redo_any);
			}
		}
		if (sdlType->attributes) {
			sdlAttributePtr *attr;
			HashPosition pos;

			zend_hash_internal_pointer_reset_ex(sdlType->attributes, &pos);
			while (zend_hash_get_current_data_ex(sdlType->attributes, (void**)&attr, &pos) == SUCCESS) {
				if ((*attr)->name) {
					xmlAttrPtr val = get_attribute(data->properties, (*attr)->name);
					char *str_val = NULL;

					if (val && val->children && val->children->content) {
						str_val = (char*)val->children->content;
						if ((*attr)->fixed && strcmp((*attr)->fixed, str_val) != 0) {
							soap_error3(E_ERROR, "Encoding: Attribute '%s' has fixed value '%s' (value '%s' is not allowed)", (*attr)->name, (*attr)->fixed, str_val);
						}
					} else if ((*attr)->fixed) {
						str_val = (*attr)->fixed;
					} else if ((*attr)->def) {
						str_val = (*attr)->def;
					}
					if (str_val) {
						xmlNodePtr dummy, text;
						zval *data;

						dummy = xmlNewNode(NULL, BAD_CAST("BOGUS"));
						text = xmlNewText(BAD_CAST(str_val));
						xmlAddChild(dummy, text);
						data = master_to_zval((*attr)->encode, dummy TSRMLS_CC);
						xmlFreeNode(dummy);
						set_zval_property(ret, (*attr)->name, data TSRMLS_CC);
					}
				}
				zend_hash_move_forward_ex(sdlType->attributes, &pos);
			}
		}
	} else {

		ALLOC_INIT_ZVAL(ret);
		FIND_XML_NULL(data, ret);
		if (soap_check_xml_ref(&ret, data TSRMLS_CC)) {
			return ret;
		}

		object_init_ex(ret, ce);
		trav = data->children;

		while (trav != NULL) {
			if (trav->type == XML_ELEMENT_NODE) {
				zval  *tmpVal;
				zval *prop;

				tmpVal = master_to_zval(NULL, trav TSRMLS_CC);

				prop = get_zval_property(ret, (char*)trav->name TSRMLS_CC);
				if (!prop) {
					if (!trav->next || !get_node(trav->next, (char*)trav->name)) {
						set_zval_property(ret, (char*)trav->name, tmpVal TSRMLS_CC);
					} else {
						zval *arr;

						MAKE_STD_ZVAL(arr);
						array_init(arr);
						add_next_index_zval(arr, tmpVal);
						set_zval_property(ret, (char*)trav->name, arr TSRMLS_CC);
					}
				} else {
					/* Property already exist - make array */
					if (Z_TYPE_P(prop) != IS_ARRAY) {
						/* Convert into array */
						zval *arr;

						MAKE_STD_ZVAL(arr);
						array_init(arr);
						Z_ADDREF_P(prop);
						add_next_index_zval(arr, prop);
						set_zval_property(ret, (char*)trav->name, arr TSRMLS_CC);
						prop = arr;
					}
					/* Add array element */
					add_next_index_zval(prop, tmpVal);
				}
			}
			trav = trav->next;
		}
	}
	return ret;
}