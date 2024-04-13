ZEND_API int zend_disable_function(char *function_name, uint function_name_length TSRMLS_DC) /* {{{ */
{
	zend_internal_function *func;
	if (zend_hash_find(CG(function_table), function_name, function_name_length+1, (void **)&func)==SUCCESS) {
		func->arg_info = NULL;
		func->handler = ZEND_FN(display_disabled_function);
		return SUCCESS;
	}
	return FAILURE;
}