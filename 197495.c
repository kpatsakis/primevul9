static void curlfile_get_property(char *name, INTERNAL_FUNCTION_PARAMETERS)
{
	zval *res;
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	res = zend_read_property(curl_CURLFile_class, getThis(), name, strlen(name), 1 TSRMLS_CC);
	*return_value = *res;
	zval_copy_ctor(return_value);
	INIT_PZVAL(return_value);
}