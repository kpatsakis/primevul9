ZEND_API int zend_startup_modules(TSRMLS_D) /* {{{ */
{
	zend_hash_sort(&module_registry, zend_sort_modules, NULL, 0 TSRMLS_CC);
	zend_hash_apply(&module_registry, (apply_func_t)zend_startup_module_int TSRMLS_CC);
	return SUCCESS;
}