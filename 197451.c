ZEND_API zend_bool zend_make_callable(zval *callable, char **callable_name TSRMLS_DC) /* {{{ */
{
	zend_fcall_info_cache fcc;

	if (zend_is_callable_ex(callable, NULL, IS_CALLABLE_STRICT, callable_name, NULL, &fcc, NULL TSRMLS_CC)) {
		if (Z_TYPE_P(callable) == IS_STRING && fcc.calling_scope) {
			zval_dtor(callable);
			array_init(callable);
			add_next_index_string(callable, fcc.calling_scope->name, 1);
			add_next_index_string(callable, fcc.function_handler->common.function_name, 1);
		}
		if (fcc.function_handler &&
			((fcc.function_handler->type == ZEND_INTERNAL_FUNCTION &&
		      (fcc.function_handler->common.fn_flags & ZEND_ACC_CALL_VIA_HANDLER)) ||
		     fcc.function_handler->type == ZEND_OVERLOADED_FUNCTION_TEMPORARY ||
		     fcc.function_handler->type == ZEND_OVERLOADED_FUNCTION)) {
			if (fcc.function_handler->type != ZEND_OVERLOADED_FUNCTION) {
				efree((char*)fcc.function_handler->common.function_name);
			}
			efree(fcc.function_handler);
		}
		return 1;
	}
	return 0;
}