ZEND_METHOD(CURLFile, setMimeType)
{
	curlfile_set_property("mime", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}