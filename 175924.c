static xmlNodePtr to_xml_gyear(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	return to_xml_datetime_ex(type, data, "%Y", style, parent TSRMLS_CC);
}