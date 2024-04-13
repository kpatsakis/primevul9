static xmlNodePtr to_xml_date(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	return to_xml_datetime_ex(type, data, "%Y-%m-%d", style, parent TSRMLS_CC);
}