static int model_to_xml_object(xmlNodePtr node, sdlContentModelPtr model, zval *object, int style, int strict TSRMLS_DC)
{
	switch (model->kind) {
		case XSD_CONTENT_ELEMENT: {
			zval *data;
			xmlNodePtr property;
			encodePtr enc;

			data = get_zval_property(object, model->u.element->name TSRMLS_CC);
			if (data &&
			    Z_TYPE_P(data) == IS_NULL &&
			    !model->u.element->nillable &&
			    model->min_occurs > 0 &&
			    !strict) {
				return 0;
			}
			if (data) {
				enc = model->u.element->encode;
				if ((model->max_occurs == -1 || model->max_occurs > 1) &&
				    Z_TYPE_P(data) == IS_ARRAY &&
				    !is_map(data)) {
					HashTable *ht = Z_ARRVAL_P(data);
					zval **val;

					zend_hash_internal_pointer_reset(ht);
					while (zend_hash_get_current_data(ht,(void**)&val) == SUCCESS) {
						if (Z_TYPE_PP(val) == IS_NULL && model->u.element->nillable) {
							property = xmlNewNode(NULL, BAD_CAST("BOGUS"));
							xmlAddChild(node, property);
							set_xsi_nil(property);
						} else {
							property = master_to_xml(enc, *val, style, node TSRMLS_CC);
							if (property->children && property->children->content &&
							    model->u.element->fixed && strcmp(model->u.element->fixed, (char*)property->children->content) != 0) {
								soap_error3(E_ERROR, "Encoding: Element '%s' has fixed value '%s' (value '%s' is not allowed)", model->u.element->name, model->u.element->fixed, property->children->content);
							}
						}
						xmlNodeSetName(property, BAD_CAST(model->u.element->name));
						if (style == SOAP_LITERAL &&
						    model->u.element->namens &&
						    model->u.element->form == XSD_FORM_QUALIFIED) {
							xmlNsPtr nsp = encode_add_ns(property, model->u.element->namens);
							xmlSetNs(property, nsp);
						}
						zend_hash_move_forward(ht);
					}
				} else {
					if (Z_TYPE_P(data) == IS_NULL && model->u.element->nillable) {
						property = xmlNewNode(NULL, BAD_CAST("BOGUS"));
						xmlAddChild(node, property);
						set_xsi_nil(property);
					} else if (Z_TYPE_P(data) == IS_NULL && model->min_occurs == 0) {
						return 1;
					} else {
						property = master_to_xml(enc, data, style, node TSRMLS_CC);
						if (property->children && property->children->content &&
						    model->u.element->fixed && strcmp(model->u.element->fixed, (char*)property->children->content) != 0) {
							soap_error3(E_ERROR, "Encoding: Element '%s' has fixed value '%s' (value '%s' is not allowed)", model->u.element->name, model->u.element->fixed, property->children->content);
						}
					}
					xmlNodeSetName(property, BAD_CAST(model->u.element->name));
					if (style == SOAP_LITERAL &&
					    model->u.element->namens &&
					    model->u.element->form == XSD_FORM_QUALIFIED) {
						xmlNsPtr nsp = encode_add_ns(property, model->u.element->namens);
						xmlSetNs(property, nsp);
					}
				}
				return 1;
			} else if (strict && model->u.element->nillable && model->min_occurs > 0) {
				property = xmlNewNode(NULL, BAD_CAST(model->u.element->name));
				xmlAddChild(node, property);
				set_xsi_nil(property);
				if (style == SOAP_LITERAL &&
				    model->u.element->namens &&
				    model->u.element->form == XSD_FORM_QUALIFIED) {
					xmlNsPtr nsp = encode_add_ns(property, model->u.element->namens);
					xmlSetNs(property, nsp);
				}
				return 1;
			} else if (model->min_occurs == 0) {
				return 2;
			} else {
				if (strict) {
					soap_error1(E_ERROR,  "Encoding: object has no '%s' property", model->u.element->name);
				}
				return 0;
			}
			break;
		}
		case XSD_CONTENT_ANY: {
			zval *data;
			xmlNodePtr property;
			encodePtr enc;

			data = get_zval_property(object, "any" TSRMLS_CC);
			if (data) {
				enc = get_conversion(XSD_ANYXML);
				if ((model->max_occurs == -1 || model->max_occurs > 1) &&
				    Z_TYPE_P(data) == IS_ARRAY &&
				    !is_map(data)) {
					HashTable *ht = Z_ARRVAL_P(data);
					zval **val;

					zend_hash_internal_pointer_reset(ht);
					while (zend_hash_get_current_data(ht,(void**)&val) == SUCCESS) {
						property = master_to_xml(enc, *val, style, node TSRMLS_CC);
						zend_hash_move_forward(ht);
					}
				} else {
					property = master_to_xml(enc, data, style, node TSRMLS_CC);
				}
				return 1;
			} else if (model->min_occurs == 0) {
				return 2;
			} else {
				if (strict) {
					soap_error0(E_ERROR,  "Encoding: object has no 'any' property");
				}
				return 0;
			}
			break;
		}
		case XSD_CONTENT_SEQUENCE:
		case XSD_CONTENT_ALL: {
			sdlContentModelPtr *tmp;
			HashPosition pos;

			zend_hash_internal_pointer_reset_ex(model->u.content, &pos);
			while (zend_hash_get_current_data_ex(model->u.content, (void**)&tmp, &pos) == SUCCESS) {
				if (!model_to_xml_object(node, *tmp, object, style, strict && ((*tmp)->min_occurs > 0) TSRMLS_CC)) {
					if (!strict || (*tmp)->min_occurs > 0) {
						return 0;
					}
				}
				strict = 1;
				zend_hash_move_forward_ex(model->u.content, &pos);
			}
			return 1;
		}
		case XSD_CONTENT_CHOICE: {
			sdlContentModelPtr *tmp;
			HashPosition pos;
			int ret = 0;

			zend_hash_internal_pointer_reset_ex(model->u.content, &pos);
			while (zend_hash_get_current_data_ex(model->u.content, (void**)&tmp, &pos) == SUCCESS) {
				int tmp_ret = model_to_xml_object(node, *tmp, object, style, 0 TSRMLS_CC);
				if (tmp_ret == 1) {
					return 1;
				} else if (tmp_ret != 0) {
					ret = 1;
				}
				zend_hash_move_forward_ex(model->u.content, &pos);
			}
			return ret;
		}
		case XSD_CONTENT_GROUP: {
			return model_to_xml_object(node, model->u.group->model, object, style, strict && model->min_occurs > 0 TSRMLS_CC);
		}
		default:
		  break;
	}
	return 1;
}