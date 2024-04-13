static xmlNodePtr to_xml_base64(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	xmlNodePtr ret, text;
	unsigned char *str;
	int str_len;

	ret = xmlNewNode(NULL, BAD_CAST("BOGUS"));
	xmlAddChild(parent, ret);
	FIND_ZVAL_NULL(data, ret, style);

	if (Z_TYPE_P(data) == IS_STRING) {
		str = php_base64_encode((unsigned char*)Z_STRVAL_P(data), Z_STRLEN_P(data), &str_len);
		text = xmlNewTextLen(str, str_len);
		xmlAddChild(ret, text);
		efree(str);
	} else {
		zval tmp = *data;

		zval_copy_ctor(&tmp);
		convert_to_string(&tmp);
		str = php_base64_encode((unsigned char*)Z_STRVAL(tmp), Z_STRLEN(tmp), &str_len);
		text = xmlNewTextLen(str, str_len);
		xmlAddChild(ret, text);
		efree(str);
		zval_dtor(&tmp);
	}

	if (style == SOAP_ENCODED) {
		set_ns_and_type(ret, type);
	}
	return ret;
}