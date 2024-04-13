static xmlNodePtr to_xml_gmonthday(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	return to_xml_datetime_ex(type, data, "--%m-%d", style, parent TSRMLS_CC);
}