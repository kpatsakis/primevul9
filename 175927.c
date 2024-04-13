static xmlNodePtr to_xml_double(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	xmlNodePtr ret;
	zval tmp;
	char *str;

	ret = xmlNewNode(NULL, BAD_CAST("BOGUS"));
	xmlAddChild(parent, ret);
	FIND_ZVAL_NULL(data, ret, style);

	tmp = *data;
	if (Z_TYPE(tmp) != IS_DOUBLE) {
		zval_copy_ctor(&tmp);
		convert_to_double(&tmp);
	}
	
	str = (char *) safe_emalloc(EG(precision), 1, MAX_LENGTH_OF_DOUBLE + 1);
	php_gcvt(Z_DVAL(tmp), EG(precision), '.', 'E', str);
	xmlNodeSetContentLen(ret, BAD_CAST(str), strlen(str));
	efree(str);

	if (style == SOAP_ENCODED) {
		set_ns_and_type(ret, type);
	}
	return ret;
}