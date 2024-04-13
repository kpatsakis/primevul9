static PHP_MSHUTDOWN_FUNCTION(libxml)
{
	if (!_php_libxml_per_request_initialization) {
		xmlSetGenericErrorFunc(NULL, NULL);

		xmlParserInputBufferCreateFilenameDefault(NULL);
		xmlOutputBufferCreateFilenameDefault(NULL);
	}
	php_libxml_shutdown();

	return SUCCESS;
}