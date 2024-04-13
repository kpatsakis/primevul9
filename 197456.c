ZEND_API int zend_parse_method_parameters_ex(int flags, int num_args TSRMLS_DC, zval *this_ptr, const char *type_spec, ...) /* {{{ */
{
	va_list va;
	int retval;
	const char *p = type_spec;
	zval **object;
	zend_class_entry *ce;
	int quiet = flags & ZEND_PARSE_PARAMS_QUIET;

	if (!this_ptr) {
		RETURN_IF_ZERO_ARGS(num_args, p, quiet);

		va_start(va, type_spec);
		retval = zend_parse_va_args(num_args, type_spec, &va, flags TSRMLS_CC);
		va_end(va);
	} else {
		p++;
		RETURN_IF_ZERO_ARGS(num_args, p, quiet);

		va_start(va, type_spec);

		object = va_arg(va, zval **);
		ce = va_arg(va, zend_class_entry *);
		*object = this_ptr;

		if (ce && !instanceof_function(Z_OBJCE_P(this_ptr), ce TSRMLS_CC)) {
			if (!quiet) {
				zend_error(E_CORE_ERROR, "%s::%s() must be derived from %s::%s",
					ce->name, get_active_function_name(TSRMLS_C), Z_OBJCE_P(this_ptr)->name, get_active_function_name(TSRMLS_C));
			}
			va_end(va);
			return FAILURE;
		}

		retval = zend_parse_va_args(num_args, p, &va, flags TSRMLS_CC);
		va_end(va);
	}
	return retval;
}