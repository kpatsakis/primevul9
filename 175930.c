static xmlNodePtr to_xml_bool(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	xmlNodePtr ret;

	ret = xmlNewNode(NULL, BAD_CAST("BOGUS"));
	xmlAddChild(parent, ret);
	FIND_ZVAL_NULL(data, ret, style);

	if (zend_is_true(data)) {
		xmlNodeSetContent(ret, BAD_CAST("true"));
	} else {
		xmlNodeSetContent(ret, BAD_CAST("false"));
	}

	if (style == SOAP_ENCODED) {
		set_ns_and_type(ret, type);
	}
	return ret;
}