static zval *to_zval_base64(encodeTypePtr type, xmlNodePtr data TSRMLS_DC)
{
	zval *ret;
	char *str;
	int str_len;

	MAKE_STD_ZVAL(ret);
	FIND_XML_NULL(data, ret);
	if (data && data->children) {
		if (data->children->type == XML_TEXT_NODE && data->children->next == NULL) {
			whiteSpace_collapse(data->children->content);
			str = (char*)php_base64_decode(data->children->content, strlen((char*)data->children->content), &str_len);
			if (!str) {
				soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
			}
			ZVAL_STRINGL(ret, str, str_len, 0);
		} else if (data->children->type == XML_CDATA_SECTION_NODE && data->children->next == NULL) {
			str = (char*)php_base64_decode(data->children->content, strlen((char*)data->children->content), &str_len);
			if (!str) {
				soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
			}
			ZVAL_STRINGL(ret, str, str_len, 0);
		} else {
			soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
		}
	} else {
		ZVAL_EMPTY_STRING(ret);
	}
	return ret;
}