ZEND_API void zend_wrong_param_count(TSRMLS_D) /* {{{ */
{
	const char *space;
	const char *class_name = get_active_class_name(&space TSRMLS_CC);

	zend_error(E_WARNING, "Wrong parameter count for %s%s%s()", class_name, space, get_active_function_name(TSRMLS_C));
}