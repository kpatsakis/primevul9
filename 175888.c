static zval *to_zval_double(encodeTypePtr type, xmlNodePtr data TSRMLS_DC)
{
	zval *ret;
	MAKE_STD_ZVAL(ret);
	FIND_XML_NULL(data, ret);

	if (data && data->children) {
		if (data->children->type == XML_TEXT_NODE && data->children->next == NULL) {
			long lval;
			double dval;

			whiteSpace_collapse(data->children->content);
			switch (is_numeric_string((char*)data->children->content, strlen((char*)data->children->content), &lval, &dval, 0)) {
				case IS_LONG:
					Z_TYPE_P(ret) = IS_DOUBLE;
					Z_DVAL_P(ret) = lval;
					break;
				case IS_DOUBLE:
					Z_TYPE_P(ret) = IS_DOUBLE;
					Z_DVAL_P(ret) = dval;
					break;
				default:
					if (strncasecmp((char*)data->children->content, "NaN", sizeof("NaN")-1) == 0) {
						ZVAL_DOUBLE(ret, php_get_nan());
					} else if (strncasecmp((char*)data->children->content, "INF", sizeof("INF")-1) == 0) {
						ZVAL_DOUBLE(ret, php_get_inf());
					} else if (strncasecmp((char*)data->children->content, "-INF", sizeof("-INF")-1) == 0) {
						ZVAL_DOUBLE(ret, -php_get_inf());
					} else {
						soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
					}
			}
		} else {
			soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
		}
	} else {
		ZVAL_NULL(ret);
	}
	return ret;
}