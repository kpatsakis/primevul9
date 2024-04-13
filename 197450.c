ZEND_METHOD(CURLFile, setPostFilename)
{
	curlfile_set_property("postname", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}