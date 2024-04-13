ZEND_METHOD(CURLFile, getFilename)
{
	curlfile_get_property("name", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}