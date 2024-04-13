PHP_FUNCTION(imagescale)
{
	zval *IM;
	gdImagePtr im;
	gdImagePtr im_scaled = NULL;
	int new_width, new_height;
	long tmp_w, tmp_h=-1, tmp_m = GD_BILINEAR_FIXED;
	gdInterpolationMethod method;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl|ll", &IM, &tmp_w, &tmp_h, &tmp_m) == FAILURE)  {
		return;
	}
	method = tmp_m;

	ZEND_FETCH_RESOURCE(im, gdImagePtr, &IM, -1, "Image", le_gd);

	if (tmp_h < 0) {
		/* preserve ratio */
		long src_x, src_y;

		src_x = gdImageSX(im);
		src_y = gdImageSY(im);
		if (src_x) {
			tmp_h = tmp_w * src_y / src_x;
		}
	}

	if (tmp_h <= 0 || tmp_w <= 0) {
		RETURN_FALSE;
	}

	new_width = tmp_w;
	new_height = tmp_h;

	if (gdImageSetInterpolationMethod(im, method)) {
		im_scaled = gdImageScale(im, new_width, new_height);
	}

	if (im_scaled == NULL) {
		RETURN_FALSE;
	} else {
		ZEND_REGISTER_RESOURCE(return_value, im_scaled, le_gd);
	}
}