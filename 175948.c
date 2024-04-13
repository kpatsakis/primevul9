static void add_xml_array_elements(xmlNodePtr xmlParam,
                                   sdlTypePtr type,
                                   encodePtr enc,
                                   xmlNsPtr ns,
                                   int dimension ,
                                   int* dims,
                                   zval* data,
                                   int style
                                   TSRMLS_DC)
{
	int j;

	if (data && Z_TYPE_P(data) == IS_ARRAY) {
	 	zend_hash_internal_pointer_reset(data->value.ht);
		for (j=0; j<dims[0]; j++) {
 			zval **zdata;

 			if (zend_hash_get_current_data(data->value.ht, (void **)&zdata) != SUCCESS) {
 				zdata = NULL;
 			}
 			if (dimension == 1) {
	 			xmlNodePtr xparam;

	 			if (zdata) {
	 				if (enc == NULL) {
 						xparam = master_to_xml(get_conversion((*zdata)->type), (*zdata), style, xmlParam TSRMLS_CC);
 					} else {
 						xparam = master_to_xml(enc, (*zdata), style, xmlParam TSRMLS_CC);
		 			}
		 		} else {
					xparam = xmlNewNode(NULL, BAD_CAST("BOGUS"));
					xmlAddChild(xmlParam, xparam);
		 		}

	 			if (type) {
 					xmlNodeSetName(xparam, BAD_CAST(type->name));
 				} else if (style == SOAP_LITERAL && enc && enc->details.type_str) {
					xmlNodeSetName(xparam, BAD_CAST(enc->details.type_str));
					xmlSetNs(xparam, ns);
				} else {
 					xmlNodeSetName(xparam, BAD_CAST("item"));
 				}
 			} else {
 				if (zdata) {
	 			  add_xml_array_elements(xmlParam, type, enc, ns, dimension-1, dims+1, *zdata, style TSRMLS_CC);
	 			} else {
	 			  add_xml_array_elements(xmlParam, type, enc, ns, dimension-1, dims+1, NULL, style TSRMLS_CC);
	 			}
 			}
 			zend_hash_move_forward(data->value.ht);
 		}
 	} else {
		for (j=0; j<dims[0]; j++) {
 			if (dimension == 1) {
	 			xmlNodePtr xparam;

				xparam = xmlNewNode(NULL, BAD_CAST("BOGUS"));
				xmlAddChild(xmlParam, xparam);
	 			if (type) {
 					xmlNodeSetName(xparam, BAD_CAST(type->name));
 				} else if (style == SOAP_LITERAL && enc && enc->details.type_str) {
					xmlNodeSetName(xparam, BAD_CAST(enc->details.type_str));
					xmlSetNs(xparam, ns);
				} else {
 					xmlNodeSetName(xparam, BAD_CAST("item"));
 				}
 			} else {
 			  add_xml_array_elements(xmlParam, type, enc, ns, dimension-1, dims+1, NULL, style TSRMLS_CC);
 			}
		}
 	}
}