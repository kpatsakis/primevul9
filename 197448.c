static void curlfile_set_property(char *name, INTERNAL_FUNCTION_PARAMETERS)
{
	char *arg = NULL;
	int arg_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}
	zend_update_property_string(curl_CURLFile_class, getThis(), name, strlen(name), arg TSRMLS_CC);
}