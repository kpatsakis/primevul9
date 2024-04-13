PHP_FUNCTION(imagerotate)
{
	zval *SIM;
	gdImagePtr im_dst, im_src;
	double degrees;
	long color;
	long ignoretransparent = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rdl|l", &SIM, &degrees, &color, &ignoretransparent) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(im_src, gdImagePtr, &SIM, -1, "Image", le_gd);

	im_dst = gdImageRotateInterpolated(im_src, (const float)degrees, color);

	if (im_dst != NULL) {
		ZEND_REGISTER_RESOURCE(return_value, im_dst, le_gd);
	} else {
		RETURN_FALSE;
	}
}