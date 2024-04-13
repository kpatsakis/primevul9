static zend_long php_session_gc(zend_bool immediate) /* {{{ */
{
	int nrand;
	zend_long num = -1;

	/* GC must be done before reading session data. */
	if ((PS(mod_data) || PS(mod_user_implemented))) {
		if (immediate) {
			PS(mod)->s_gc(&PS(mod_data), PS(gc_maxlifetime), &num);
			return num;
		}
		nrand = (zend_long) ((float) PS(gc_divisor) * php_combined_lcg());
		if (PS(gc_probability) > 0 && nrand < PS(gc_probability)) {
			PS(mod)->s_gc(&PS(mod_data), PS(gc_maxlifetime), &num);
		}
	}
	return num;
} /* }}} */