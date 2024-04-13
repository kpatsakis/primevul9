static int zend_parse_arg(int arg_num, zval **arg, va_list *va, const char **spec, int quiet TSRMLS_DC) /* {{{ */
{
	const char *expected_type = NULL;
	char *error = NULL;
	int severity = E_WARNING;

	expected_type = zend_parse_arg_impl(arg_num, arg, va, spec, &error, &severity TSRMLS_CC);
	if (expected_type) {
		if (!quiet && (*expected_type || error)) {
			const char *space;
			const char *class_name = get_active_class_name(&space TSRMLS_CC);

			if (error) {
				zend_error(severity, "%s%s%s() expects parameter %d %s",
						class_name, space, get_active_function_name(TSRMLS_C), arg_num, error);
				efree(error);
			} else {
				zend_error(severity, "%s%s%s() expects parameter %d to be %s, %s given",
						class_name, space, get_active_function_name(TSRMLS_C), arg_num, expected_type,
						zend_zval_type_name(*arg));
			}
		}
		if (severity != E_STRICT) {
			return FAILURE;
		}
	}

	return SUCCESS;
}