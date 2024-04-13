static PHP_FUNCTION(libxml_use_internal_errors)
{
	xmlStructuredErrorFunc current_handler;
	zend_bool use_errors=0, retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &use_errors) == FAILURE) {
		return;
	}

	current_handler = xmlStructuredError;
	if (current_handler && current_handler == php_libxml_structured_error_handler) {
		retval = 1;
	} else {
		retval = 0;
	}

	if (ZEND_NUM_ARGS() == 0) {
		RETURN_BOOL(retval);
	}

	if (use_errors == 0) {
		xmlSetStructuredErrorFunc(NULL, NULL);
		if (LIBXML(error_list)) {
			zend_llist_destroy(LIBXML(error_list));
			efree(LIBXML(error_list));
			LIBXML(error_list) = NULL;
		}
	} else {
		xmlSetStructuredErrorFunc(NULL, php_libxml_structured_error_handler);
		if (LIBXML(error_list) == NULL) {
			LIBXML(error_list) = (zend_llist *) emalloc(sizeof(zend_llist));
			zend_llist_init(LIBXML(error_list), sizeof(xmlError), (llist_dtor_func_t) _php_libxml_free_error, 0);
		}
	}
	RETURN_BOOL(retval);
}