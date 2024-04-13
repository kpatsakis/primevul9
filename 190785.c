PHP_FUNCTION(sys_get_temp_dir)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	RETURN_STRING((char *)php_get_temporary_directory(TSRMLS_C), 1);
}