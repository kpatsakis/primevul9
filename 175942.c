static zval *to_zval_stringc(encodeTypePtr type, xmlNodePtr data TSRMLS_DC)
{
	zval *ret;
	MAKE_STD_ZVAL(ret);
	FIND_XML_NULL(data, ret);
	if (data && data->children) {
		if (data->children->type == XML_TEXT_NODE && data->children->next == NULL) {
			whiteSpace_collapse(data->children->content);
			if (SOAP_GLOBAL(encoding) != NULL) {
				xmlBufferPtr in  = xmlBufferCreateStatic(data->children->content, xmlStrlen(data->children->content));
				xmlBufferPtr out = xmlBufferCreate();
				int n = xmlCharEncOutFunc(SOAP_GLOBAL(encoding), out, in);

				if (n >= 0) {
					ZVAL_STRING(ret, (char*)xmlBufferContent(out), 1);
				} else {
					ZVAL_STRING(ret, (char*)data->children->content, 1);
				}
				xmlBufferFree(out);
				xmlBufferFree(in);
			} else {
				ZVAL_STRING(ret, (char*)data->children->content, 1);
			}
		} else if (data->children->type == XML_CDATA_SECTION_NODE && data->children->next == NULL) {
			ZVAL_STRING(ret, (char*)data->children->content, 1);
		} else {
			soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
		}
	} else {
		ZVAL_EMPTY_STRING(ret);
	}
	return ret;
}