static xmlNodePtr guess_array_map(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	encodePtr enc = NULL;

	if (data && Z_TYPE_P(data) == IS_ARRAY) {
		if (is_map(data)) {
			enc = get_conversion(APACHE_MAP);
		} else {
			enc = get_conversion(SOAP_ENC_ARRAY);
		}
	}
	if (!enc) {
		enc = get_conversion(IS_NULL);
	}

	return master_to_xml(enc, data, style, parent TSRMLS_CC);
}