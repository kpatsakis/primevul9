xmlNodePtr to_xml_user(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	xmlNodePtr ret = NULL;
	zval *return_value;

	if (type && type->map && type->map->to_xml) {
		MAKE_STD_ZVAL(return_value);

		if (call_user_function(EG(function_table), NULL, type->map->to_xml, return_value, 1, &data TSRMLS_CC) == FAILURE) {
			soap_error0(E_ERROR, "Encoding: Error calling to_xml callback");
		}
		if (Z_TYPE_P(return_value) == IS_STRING) {		
			xmlDocPtr doc = soap_xmlParseMemory(Z_STRVAL_P(return_value), Z_STRLEN_P(return_value));
			if (doc && doc->children) {				
				ret = xmlDocCopyNode(doc->children, parent->doc, 1);
			}
			xmlFreeDoc(doc);
		}

		zval_ptr_dtor(&return_value);
	}
	if (!ret) {
		ret = xmlNewNode(NULL, BAD_CAST("BOGUS"));
	}
	xmlAddChild(parent, ret);
	if (style == SOAP_ENCODED) {
		set_ns_and_type(ret, type);
	}
	return ret;
}