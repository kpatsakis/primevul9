
static int _display_module_info_def(zend_module_entry *module TSRMLS_DC) /* {{{ */
{
	if (!module->info_func && !module->version) {
		php_info_print_module(module TSRMLS_CC);
	}
	return ZEND_HASH_APPLY_KEEP;