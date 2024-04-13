ZEND_METHOD(CURLFile, getMimeType)
{
	curlfile_get_property("mime", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}