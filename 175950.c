static zval *to_zval_object(encodeTypePtr type, xmlNodePtr data TSRMLS_DC)
{
	return to_zval_object_ex(type, data, NULL TSRMLS_CC);
}