static void curlfile_ctor(INTERNAL_FUNCTION_PARAMETERS)
{
	char *fname = NULL, *mime = NULL, *postname = NULL;
	int fname_len, mime_len, postname_len;
	zval *cf = return_value;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ss", &fname, &fname_len, &mime, &mime_len, &postname, &postname_len) == FAILURE) {
		return;
	}

	if (fname) {
		zend_update_property_string(curl_CURLFile_class, cf, "name", sizeof("name")-1, fname TSRMLS_CC);
	}

	if (mime) {
		zend_update_property_string(curl_CURLFile_class, cf, "mime", sizeof("mime")-1, mime TSRMLS_CC);
	}

	if (postname) {
		zend_update_property_string(curl_CURLFile_class, cf, "postname", sizeof("postname")-1, postname TSRMLS_CC);
	}
}