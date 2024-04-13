static xmlNodePtr to_xml_hexbin(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	static char hexconvtab[] = "0123456789ABCDEF";
	xmlNodePtr ret, text;
	unsigned char *str;
	zval tmp;
	int i, j;

	ret = xmlNewNode(NULL, BAD_CAST("BOGUS"));
	xmlAddChild(parent, ret);
	FIND_ZVAL_NULL(data, ret, style);

	if (Z_TYPE_P(data) != IS_STRING) {
		tmp = *data;
		zval_copy_ctor(&tmp);
		convert_to_string(&tmp);
		data = &tmp;
	}
	str = (unsigned char *) safe_emalloc(Z_STRLEN_P(data) * 2, sizeof(char), 1);

	for (i = j = 0; i < Z_STRLEN_P(data); i++) {
		str[j++] = hexconvtab[((unsigned char)Z_STRVAL_P(data)[i]) >> 4];
		str[j++] = hexconvtab[((unsigned char)Z_STRVAL_P(data)[i]) & 15];
	}
	str[j] = '\0';

	text = xmlNewTextLen(str, Z_STRLEN_P(data) * 2 * sizeof(char));
	xmlAddChild(ret, text);
	efree(str);
	if (data == &tmp) {
		zval_dtor(&tmp);
	}

	if (style == SOAP_ENCODED) {
		set_ns_and_type(ret, type);
	}
	return ret;
}