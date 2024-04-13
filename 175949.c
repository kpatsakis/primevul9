static xmlNodePtr to_xml_map(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	xmlNodePtr xmlParam;
	int i;

	xmlParam = xmlNewNode(NULL, BAD_CAST("BOGUS"));
	xmlAddChild(parent, xmlParam);
	FIND_ZVAL_NULL(data, xmlParam, style);

	if (Z_TYPE_P(data) == IS_ARRAY) {
		i = zend_hash_num_elements(Z_ARRVAL_P(data));
		zend_hash_internal_pointer_reset(data->value.ht);
		for (;i > 0;i--) {
			xmlNodePtr xparam, item;
			xmlNodePtr key;
			zval **temp_data;
			char *key_val;
			ulong int_val;

			zend_hash_get_current_data(data->value.ht, (void **)&temp_data);
			item = xmlNewNode(NULL, BAD_CAST("item"));
			xmlAddChild(xmlParam, item);
			key = xmlNewNode(NULL, BAD_CAST("key"));
			xmlAddChild(item,key);
			if (zend_hash_get_current_key(data->value.ht, &key_val, &int_val, FALSE) == HASH_KEY_IS_STRING) {
				if (style == SOAP_ENCODED) {
					set_xsi_type(key, "xsd:string");
				}
				xmlNodeSetContent(key, BAD_CAST(key_val));
			} else {
				smart_str tmp = {0};
				smart_str_append_long(&tmp, int_val);
				smart_str_0(&tmp);

				if (style == SOAP_ENCODED) {
					set_xsi_type(key, "xsd:int");
				}
				xmlNodeSetContentLen(key, BAD_CAST(tmp.c), tmp.len);

				smart_str_free(&tmp);
			}

			xparam = master_to_xml(get_conversion((*temp_data)->type), (*temp_data), style, item TSRMLS_CC);
			xmlNodeSetName(xparam, BAD_CAST("value"));

			zend_hash_move_forward(data->value.ht);
		}
	}
	if (style == SOAP_ENCODED) {
		set_ns_and_type(xmlParam, type);
	}

	return xmlParam;
}