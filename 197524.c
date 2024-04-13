void module_destructor(zend_module_entry *module) /* {{{ */
{
	TSRMLS_FETCH();

	if (module->type == MODULE_TEMPORARY) {
		zend_clean_module_rsrc_dtors(module->module_number TSRMLS_CC);
		clean_module_constants(module->module_number TSRMLS_CC);
		clean_module_classes(module->module_number TSRMLS_CC);
	}

	if (module->module_started && module->module_shutdown_func) {
#if 0
		zend_printf("%s: Module shutdown\n", module->name);
#endif
		module->module_shutdown_func(module->type, module->module_number TSRMLS_CC);
	}

	/* Deinitilaise module globals */
	if (module->globals_size) {
#ifdef ZTS
		if (*module->globals_id_ptr) {
			ts_free_id(*module->globals_id_ptr);
		}
#else
		if (module->globals_dtor) {
			module->globals_dtor(module->globals_ptr TSRMLS_CC);
		}
#endif
	}

	module->module_started=0;
	if (module->functions) {
		zend_unregister_functions(module->functions, -1, NULL TSRMLS_CC);
	}

#if HAVE_LIBDL
#if !(defined(NETWARE) && defined(APACHE_1_BUILD))
	if (module->handle && !getenv("ZEND_DONT_UNLOAD_MODULES")) {
		DL_UNLOAD(module->handle);
	}
#endif
#endif
}