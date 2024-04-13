PHP_FUNCTION(iconv_get_encoding)
{
	char *type = "all";
	int type_len = sizeof("all")-1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &type, &type_len) == FAILURE)
		return;

	if (!strcasecmp("all", type)) {
		array_init(return_value);
		add_assoc_string(return_value, "input_encoding",    get_input_encoding(TSRMLS_C), 1);
		add_assoc_string(return_value, "output_encoding",   get_output_encoding(TSRMLS_C), 1);
		add_assoc_string(return_value, "internal_encoding", get_internal_encoding(TSRMLS_C), 1);
	} else if (!strcasecmp("input_encoding", type)) {
		RETVAL_STRING(get_input_encoding(TSRMLS_C), 1);
	} else if (!strcasecmp("output_encoding", type)) {
		RETVAL_STRING(get_output_encoding(TSRMLS_C), 1);
	} else if (!strcasecmp("internal_encoding", type)) {
		RETVAL_STRING(get_internal_encoding(TSRMLS_C), 1);
	} else {
		RETURN_FALSE;
	}

}