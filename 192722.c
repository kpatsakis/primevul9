PHPAPI double _php_math_round(double value, int places, int mode) {
	double f1, f2;
	double tmp_value;
	int precision_places;

	if (!php_math_is_finite(value)) {
		return value;
	}
	
	precision_places = 14 - php_intlog10abs(value);

	f1 = php_intpow10(abs(places));

	/* If the decimal precision guaranteed by FP arithmetic is higher than
	   the requested places BUT is small enough to make sure a non-zero value
	   is returned, pre-round the result to the precision */
	if (precision_places > places && precision_places - places < 15) {
		f2 = php_intpow10(abs(precision_places));
		if (precision_places >= 0) {
			tmp_value = value * f2;
		} else {
			tmp_value = value / f2;
		}
		/* preround the result (tmp_value will always be something * 1e14,
		   thus never larger than 1e15 here) */
		tmp_value = php_round_helper(tmp_value, mode);
		/* now correctly move the decimal point */
		f2 = php_intpow10(abs(places - precision_places));
		/* because places < precision_places */
		tmp_value = tmp_value / f2;
	} else {
		/* adjust the value */
		if (places >= 0) {
			tmp_value = value * f1;
		} else {
			tmp_value = value / f1;
		}
		/* This value is beyond our precision, so rounding it is pointless */
		if (fabs(tmp_value) >= 1e15) {
			return value;
		}
	}

	/* round the temp value */
	tmp_value = php_round_helper(tmp_value, mode);
	
	/* see if it makes sense to use simple division to round the value */
	if (abs(places) < 23) {
		if (places > 0) {
			tmp_value = tmp_value / f1;
		} else {
			tmp_value = tmp_value * f1;
		}
	} else {
		/* Simple division can't be used since that will cause wrong results.
		   Instead, the number is converted to a string and back again using
		   strtod(). strtod() will return the nearest possible FP value for
		   that string. */

		/* 40 Bytes should be more than enough for this format string. The
		   float won't be larger than 1e15 anyway. But just in case, use
		   snprintf() and make sure the buffer is zero-terminated */
		char buf[40];
		snprintf(buf, 39, "%15fe%d", tmp_value, -places);
		buf[39] = '\0';
		tmp_value = zend_strtod(buf, NULL);
		/* couldn't convert to string and back */
		if (!zend_finite(tmp_value) || zend_isnan(tmp_value)) {
			tmp_value = value;
		}
	}

	return tmp_value;
}