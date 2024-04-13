xmlNodePtr master_to_xml(encodePtr encode, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	return master_to_xml_int(encode, data, style, parent, 1 TSRMLS_CC);
}