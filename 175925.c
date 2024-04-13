static zval *to_zval_null(encodeTypePtr type, xmlNodePtr data TSRMLS_DC)
{
	zval *ret;
	MAKE_STD_ZVAL(ret);
	ZVAL_NULL(ret);
	return ret;
}