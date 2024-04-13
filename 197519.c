static int zend_startup_module_int(zend_module_entry *module TSRMLS_DC) /* {{{ */
{
	return (zend_startup_module_ex(module TSRMLS_CC) == SUCCESS) ? ZEND_HASH_APPLY_KEEP : ZEND_HASH_APPLY_REMOVE;
}