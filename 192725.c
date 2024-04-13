PHPAPI char * _php_math_zvaltobase(zval *arg, int base TSRMLS_DC)
{
	static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";

	if ((Z_TYPE_P(arg) != IS_LONG && Z_TYPE_P(arg) != IS_DOUBLE) || base < 2 || base > 36) {
		return STR_EMPTY_ALLOC();
	}

	if (Z_TYPE_P(arg) == IS_DOUBLE) {
		double fvalue = floor(Z_DVAL_P(arg)); /* floor it just in case */
		char *ptr, *end;
		char buf[(sizeof(double) << 3) + 1];

		/* Don't try to convert +/- infinity */
		if (fvalue == HUGE_VAL || fvalue == -HUGE_VAL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Number too large");
			return STR_EMPTY_ALLOC();
		}

		end = ptr = buf + sizeof(buf) - 1;
		*ptr = '\0';

		do {
			*--ptr = digits[(int) fmod(fvalue, base)];
			fvalue /= base;
		} while (ptr > buf && fabs(fvalue) >= 1);

		return estrndup(ptr, end - ptr);
	}
	
	return _php_math_longtobase(arg, base);
}	