static xmlNodePtr to_xml_time(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	/* TODO: microsecconds */
	return to_xml_datetime_ex(type, data, "%H:%M:%S", style, parent TSRMLS_CC);
}