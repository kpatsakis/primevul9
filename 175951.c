static zval *to_zval_long(encodeTypePtr type, xmlNodePtr data TSRMLS_DC)
{
	zval *ret;
	MAKE_STD_ZVAL(ret);
	FIND_XML_NULL(data, ret);

	if (data && data->children) {
		if (data->children->type == XML_TEXT_NODE && data->children->next == NULL) {
			long lval;
			double dval;

			whiteSpace_collapse(data->children->content);
			errno = 0;

			switch ((Z_TYPE_P(ret) = is_numeric_string((char*)data->children->content, strlen((char*)data->children->content), &lval, &dval, 0))) {
				case IS_LONG:
					Z_LVAL_P(ret) = lval;
					break;
				case IS_DOUBLE:
					Z_DVAL_P(ret) = dval;
					break;
				default:
					soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
			}
		} else {
			soap_error0(E_ERROR, "Encoding: Violation of encoding rules");
		}
	} else {
		ZVAL_NULL(ret);
	}
	return ret;
}