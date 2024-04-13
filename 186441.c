static int php_libxml_post_deactivate(void)
{
	TSRMLS_FETCH();
	/* reset libxml generic error handling */
	if (_php_libxml_per_request_initialization) {
		xmlSetGenericErrorFunc(NULL, NULL);

		xmlParserInputBufferCreateFilenameDefault(NULL);
		xmlOutputBufferCreateFilenameDefault(NULL);
	}
	xmlSetStructuredErrorFunc(NULL, NULL);

	if (LIBXML(stream_context)) {
		/* the steam_context resource will be released by resource list destructor */
		efree(LIBXML(stream_context));
		LIBXML(stream_context) = NULL;
	}
	smart_str_free(&LIBXML(error_buffer));
	if (LIBXML(error_list)) {
		zend_llist_destroy(LIBXML(error_list));
		efree(LIBXML(error_list));
		LIBXML(error_list) = NULL;
	}
	xmlResetLastError();
	
	_php_libxml_destroy_fci(&LIBXML(entity_loader).fci);

	return SUCCESS;
}