static zval *to_zval_hexbin(encodeTypePtr type, xmlNodePtr data TSRMLS_DC)
{
	zval *ret;
	unsigned char *str;
	int str_len, i, j;
	unsigned char c;

	MAKE_STD_ZVAL(ret);
	FIND_XML_NULL(data, ret);
	if (data && data->children) {
		if (data->children->type == XML_TEXT_NODE && data->children->next == NULL) {
			whiteSpace_collapse(data->children->content);
		} else if (data->children->type != XML_CDATA_SECTION_NODE || data->children->next != NULL) {
			soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
			return ret;
		}
		str_len = strlen((char*)data->children->content) / 2;
		str = emalloc(str_len+1);
		for (i = j = 0; i < str_len; i++) {
			c = data->children->content[j++];
			if (c >= '0' && c <= '9') {
				str[i] = (c - '0') << 4;
			} else if (c >= 'a' && c <= 'f') {
				str[i] = (c - 'a' + 10) << 4;
			} else if (c >= 'A' && c <= 'F') {
				str[i] = (c - 'A' + 10) << 4;
			} else {
				soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
			}
			c = data->children->content[j++];
			if (c >= '0' && c <= '9') {
				str[i] |= c - '0';
			} else if (c >= 'a' && c <= 'f') {
				str[i] |= c - 'a' + 10;
			} else if (c >= 'A' && c <= 'F') {
				str[i] |= c - 'A' + 10;
			} else {
				soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
			}
		}
		str[str_len] = '\0';
		ZVAL_STRINGL(ret, (char*)str, str_len, 0);
	} else {
		ZVAL_EMPTY_STRING(ret);
	}
	return ret;
}