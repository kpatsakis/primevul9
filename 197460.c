ZEND_API void zend_deactivate_modules(TSRMLS_D) /* {{{ */
{
	EG(opline_ptr) = NULL; /* we're no longer executing anything */

	zend_try {
		if (EG(full_tables_cleanup)) {
			zend_hash_reverse_apply(&module_registry, (apply_func_t) module_registry_cleanup TSRMLS_CC);
		} else {
			zend_module_entry **p = module_request_shutdown_handlers;

			while (*p) {
				zend_module_entry *module = *p;

				module->request_shutdown_func(module->type, module->module_number TSRMLS_CC);
				p++;
			}
		}
	} zend_end_try();
}