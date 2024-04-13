static xmlNodePtr to_xml_gday(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	return to_xml_datetime_ex(type, data, "---%d", style, parent TSRMLS_CC);
}