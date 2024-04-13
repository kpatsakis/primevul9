static zval *to_zval_bool(encodeTypePtr type, xmlNodePtr data TSRMLS_DC)
{
	zval *ret;
	MAKE_STD_ZVAL(ret);
	FIND_XML_NULL(data, ret);

	if (data && data->children) {
		if (data->children->type == XML_TEXT_NODE && data->children->next == NULL) {
			whiteSpace_collapse(data->children->content);
			if (stricmp((char*)data->children->content, "true") == 0 ||
				stricmp((char*)data->children->content, "t") == 0 ||
				strcmp((char*)data->children->content, "1") == 0) {
				ZVAL_BOOL(ret, 1);
			} else if (stricmp((char*)data->children->content, "false") == 0 ||
				stricmp((char*)data->children->content, "f") == 0 ||
				strcmp((char*)data->children->content, "0") == 0) {
				ZVAL_BOOL(ret, 0);
			} else {
				ZVAL_STRING(ret, (char*)data->children->content, 1);
				convert_to_boolean(ret);
			}
		} else {
			soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
		}
	} else {
		ZVAL_NULL(ret);
	}
	return ret;
}