ZEND_API int _array_init(zval *arg, uint size ZEND_FILE_LINE_DC) /* {{{ */
{
	ALLOC_HASHTABLE_REL(Z_ARRVAL_P(arg));

	_zend_hash_init(Z_ARRVAL_P(arg), size, ZVAL_PTR_DTOR, 0 ZEND_FILE_LINE_RELAY_CC);
	Z_TYPE_P(arg) = IS_ARRAY;
	return SUCCESS;
}