ZEND_API void zend_collect_module_handlers(TSRMLS_D) /* {{{ */
{
	HashPosition pos;
	zend_module_entry *module;
	int startup_count = 0;
	int shutdown_count = 0;
	int post_deactivate_count = 0;
	zend_class_entry **pce;
	int class_count = 0;

	/* Collect extensions with request startup/shutdown handlers */
	for (zend_hash_internal_pointer_reset_ex(&module_registry, &pos);
	     zend_hash_get_current_data_ex(&module_registry, (void *) &module, &pos) == SUCCESS;
	     zend_hash_move_forward_ex(&module_registry, &pos)) {
		if (module->request_startup_func) {
			startup_count++;
		}
		if (module->request_shutdown_func) {
			shutdown_count++;
		}
		if (module->post_deactivate_func) {
			post_deactivate_count++;
		}
	}
	module_request_startup_handlers = (zend_module_entry**)malloc(
	    sizeof(zend_module_entry*) *
		(startup_count + 1 +
		 shutdown_count + 1 +
		 post_deactivate_count + 1));
	module_request_startup_handlers[startup_count] = NULL;
	module_request_shutdown_handlers = module_request_startup_handlers + startup_count + 1;
	module_request_shutdown_handlers[shutdown_count] = NULL;
	module_post_deactivate_handlers = module_request_shutdown_handlers + shutdown_count + 1;
	module_post_deactivate_handlers[post_deactivate_count] = NULL;
	startup_count = 0;

	for (zend_hash_internal_pointer_reset_ex(&module_registry, &pos);
	     zend_hash_get_current_data_ex(&module_registry, (void *) &module, &pos) == SUCCESS;
	     zend_hash_move_forward_ex(&module_registry, &pos)) {
		if (module->request_startup_func) {
			module_request_startup_handlers[startup_count++] = module;
		}
		if (module->request_shutdown_func) {
			module_request_shutdown_handlers[--shutdown_count] = module;
		}
		if (module->post_deactivate_func) {
			module_post_deactivate_handlers[--post_deactivate_count] = module;
		}
	}

	/* Collect internal classes with static members */
	for (zend_hash_internal_pointer_reset_ex(CG(class_table), &pos);
	     zend_hash_get_current_data_ex(CG(class_table), (void *) &pce, &pos) == SUCCESS;
	     zend_hash_move_forward_ex(CG(class_table), &pos)) {
		if ((*pce)->type == ZEND_INTERNAL_CLASS &&
		    (*pce)->default_static_members_count > 0) {
		    class_count++;
		}
	}

	class_cleanup_handlers = (zend_class_entry**)malloc(
		sizeof(zend_class_entry*) *
		(class_count + 1));
	class_cleanup_handlers[class_count] = NULL;

	if (class_count) {
		for (zend_hash_internal_pointer_reset_ex(CG(class_table), &pos);
		     zend_hash_get_current_data_ex(CG(class_table), (void *) &pce, &pos) == SUCCESS;
	    	 zend_hash_move_forward_ex(CG(class_table), &pos)) {
			if ((*pce)->type == ZEND_INTERNAL_CLASS &&
			    (*pce)->default_static_members_count > 0) {
			    class_cleanup_handlers[--class_count] = *pce;
			}
		}
	}
}