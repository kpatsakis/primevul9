static xmlNodePtr to_xml_datetime(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	return to_xml_datetime_ex(type, data, "%Y-%m-%dT%H:%M:%S", style, parent TSRMLS_CC);
}