static xmlNodePtr to_xml_duration(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	/* TODO: '-'?P([0-9]+Y)?([0-9]+M)?([0-9]+D)?T([0-9]+H)?([0-9]+M)?([0-9]+S)? */
	return to_xml_string(type, data, style, parent TSRMLS_CC);
}