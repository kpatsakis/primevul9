static zval *to_zval_map(encodeTypePtr type, xmlNodePtr data TSRMLS_DC)
{
	zval *ret, *key, *value;
	xmlNodePtr trav, item, xmlKey, xmlValue;

	MAKE_STD_ZVAL(ret);
	FIND_XML_NULL(data, ret);

	if (data && data->children) {
		array_init(ret);
		trav = data->children;

		trav = data->children;
		FOREACHNODE(trav, "item", item) {
			xmlKey = get_node(item->children, "key");
			if (!xmlKey) {
				soap_error0(E_ERROR,  "Encoding: Can't decode apache map, missing key");
			}

			xmlValue = get_node(item->children, "value");
			if (!xmlKey) {
				soap_error0(E_ERROR,  "Encoding: Can't decode apache map, missing value");
			}

			key = master_to_zval(NULL, xmlKey TSRMLS_CC);
			value = master_to_zval(NULL, xmlValue TSRMLS_CC);

			if (Z_TYPE_P(key) == IS_STRING) {
				zend_symtable_update(Z_ARRVAL_P(ret), Z_STRVAL_P(key), Z_STRLEN_P(key) + 1, &value, sizeof(zval *), NULL);
			} else if (Z_TYPE_P(key) == IS_LONG) {
				zend_hash_index_update(Z_ARRVAL_P(ret), Z_LVAL_P(key), &value, sizeof(zval *), NULL);
			} else {
				soap_error0(E_ERROR,  "Encoding: Can't decode apache map, only Strings or Longs are allowd as keys");
			}
			zval_ptr_dtor(&key);
		}
		ENDFOREACH(trav);
	} else {
		ZVAL_NULL(ret);
	}
	return ret;
}