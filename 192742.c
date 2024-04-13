PHP_FUNCTION(floor)
{
	zval **value;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &value) == FAILURE) {
		return;
	}
	convert_scalar_to_number_ex(value);

	if (Z_TYPE_PP(value) == IS_DOUBLE) {
		RETURN_DOUBLE(floor(Z_DVAL_PP(value)));
	} else if (Z_TYPE_PP(value) == IS_LONG) {
		convert_to_double_ex(value);
		RETURN_DOUBLE(Z_DVAL_PP(value));
	}
	RETURN_FALSE;
}