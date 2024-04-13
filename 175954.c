static xmlNodePtr to_xml_union(encodeTypePtr enc, zval *data, int style, xmlNodePtr parent TSRMLS_DC) {
	/*FIXME*/
	return to_xml_list(enc,data,style, parent TSRMLS_CC);
}