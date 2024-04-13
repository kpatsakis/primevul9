ZEND_METHOD(CURLFile, getPostFilename)
{
	curlfile_get_property("postname", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}