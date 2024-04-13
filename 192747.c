PHP_FUNCTION(abs) 
{
	zval **value;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &value) == FAILURE) {
		return;
	}
	convert_scalar_to_number_ex(value);
	
	if (Z_TYPE_PP(value) == IS_DOUBLE) {
		RETURN_DOUBLE(fabs(Z_DVAL_PP(value)));
	} else if (Z_TYPE_PP(value) == IS_LONG) {
		if (Z_LVAL_PP(value) == LONG_MIN) {
			RETURN_DOUBLE(-(double)LONG_MIN);
		} else {
			RETURN_LONG(Z_LVAL_PP(value) < 0 ? -Z_LVAL_PP(value) : Z_LVAL_PP(value));
		}
	}
	RETURN_FALSE;
}