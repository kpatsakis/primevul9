ZEND_API void zend_post_deactivate_modules(TSRMLS_D) /* {{{ */
{
	if (EG(full_tables_cleanup)) {
		zend_hash_apply(&module_registry, (apply_func_t) exec_done_cb TSRMLS_CC);
		zend_hash_reverse_apply(&module_registry, (apply_func_t) module_registry_unload_temp TSRMLS_CC);
	} else {
		zend_module_entry **p = module_post_deactivate_handlers;

		while (*p) {
			zend_module_entry *module = *p;

			module->post_deactivate_func();
			p++;
		}
	}
}