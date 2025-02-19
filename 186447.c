static PHP_RINIT_FUNCTION(libxml)
{
	if (_php_libxml_per_request_initialization) {
		/* report errors via handler rather than stderr */
		xmlSetGenericErrorFunc(NULL, php_libxml_error_handler);
		xmlParserInputBufferCreateFilenameDefault(php_libxml_input_buffer_create_filename);
		xmlOutputBufferCreateFilenameDefault(php_libxml_output_buffer_create_filename);

		/* Enable the entity loader by default. This ensure that
		 * other threads/requests that might have disable the loader
		 * do not affect the current request.
		 */
		LIBXML(entity_loader_disabled) = 0;
	}
	return SUCCESS;
}