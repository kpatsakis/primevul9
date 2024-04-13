static xmlNodePtr to_xml_null(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	xmlNodePtr ret;

	ret = xmlNewNode(NULL, BAD_CAST("BOGUS"));
	xmlAddChild(parent, ret);
	if (style == SOAP_ENCODED) {
		set_xsi_nil(ret);
	}
	return ret;
}