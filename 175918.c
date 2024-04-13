static xmlNodePtr to_xml_long(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	xmlNodePtr ret;

	ret = xmlNewNode(NULL, BAD_CAST("BOGUS"));
	xmlAddChild(parent, ret);
	FIND_ZVAL_NULL(data, ret, style);

	if (Z_TYPE_P(data) == IS_DOUBLE) {
		char s[256];

		snprintf(s, sizeof(s), "%0.0F",floor(Z_DVAL_P(data)));
		xmlNodeSetContent(ret, BAD_CAST(s));
	} else {
		zval tmp = *data;

		zval_copy_ctor(&tmp);
		if (Z_TYPE(tmp) != IS_LONG) {
			convert_to_long(&tmp);
		}
		convert_to_string(&tmp);
		xmlNodeSetContentLen(ret, BAD_CAST(Z_STRVAL(tmp)), Z_STRLEN(tmp));
		zval_dtor(&tmp);
	}

	if (style == SOAP_ENCODED) {
		set_ns_and_type(ret, type);
	}
	return ret;
}