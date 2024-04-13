zval *to_zval_user(encodeTypePtr type, xmlNodePtr node TSRMLS_DC)
{
	zval *return_value;

	if (type && type->map && type->map->to_zval) {
		xmlBufferPtr buf;
		zval *data;
		xmlNodePtr copy;

		copy = xmlCopyNode(node, 1);
		buf = xmlBufferCreate();
		xmlNodeDump(buf, NULL, copy, 0, 0);
		MAKE_STD_ZVAL(data);
		ZVAL_STRING(data, (char*)xmlBufferContent(buf), 1);
		xmlBufferFree(buf);
		xmlFreeNode(copy);

		ALLOC_INIT_ZVAL(return_value);
		
		if (call_user_function(EG(function_table), NULL, type->map->to_zval, return_value, 1, &data TSRMLS_CC) == FAILURE) {
			soap_error0(E_ERROR, "Encoding: Error calling from_xml callback");
		}
		zval_ptr_dtor(&data);
	} else {
		ALLOC_INIT_ZVAL(return_value);
	}
	return return_value;
}