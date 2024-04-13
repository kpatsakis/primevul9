PHP_FUNCTION(exif_imagetype)
{
	char *imagefile;
	int imagefile_len;
	php_stream * stream;
 	int itype = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p", &imagefile, &imagefile_len) == FAILURE) {
		return;
	}

	stream = php_stream_open_wrapper(imagefile, "rb", IGNORE_PATH|REPORT_ERRORS, NULL);

	if (stream == NULL) {
		RETURN_FALSE;
	}

	itype = php_getimagetype(stream, NULL TSRMLS_CC);

	php_stream_close(stream);

	if (itype == IMAGE_FILETYPE_UNKNOWN) {
		RETURN_FALSE;
	} else {
		ZVAL_LONG(return_value, itype);
	}
}