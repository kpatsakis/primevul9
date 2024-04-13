ZEND_API void zend_activate_modules(TSRMLS_D) /* {{{ */
{
	zend_module_entry **p = module_request_startup_handlers;

	while (*p) {
		zend_module_entry *module = *p;

		if (module->request_startup_func(module->type, module->module_number TSRMLS_CC)==FAILURE) {
			zend_error(E_WARNING, "request_startup() for %s module failed", module->name);
			exit(1);
		}
		p++;
	}
}