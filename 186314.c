static void php_session_normalize_vars() /* {{{ */
{
	PS_ENCODE_VARS;

	IF_SESSION_VARS() {
		PS_ENCODE_LOOP(
			if (Z_TYPE_P(struc) == IS_PTR) {
				zval *zv = (zval *)Z_PTR_P(struc);
				ZVAL_COPY_VALUE(struc, zv);
				ZVAL_UNDEF(zv);
			}
		);
	}
}