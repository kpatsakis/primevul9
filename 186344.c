static PHP_FUNCTION(session_set_save_handler)
{
	zval *args = NULL;
	int i, num_args, argc = ZEND_NUM_ARGS();
	zend_string *ini_name, *ini_val;

	if (PS(session_status) == php_session_active) {
		php_error_docref(NULL, E_WARNING, "Cannot change save handler when session is active");
		RETURN_FALSE;
	}

	if (SG(headers_sent)) {
		php_error_docref(NULL, E_WARNING, "Cannot change save handler when headers already sent");
		RETURN_FALSE;
	}

	if (argc > 0 && argc <= 2) {
		zval *obj = NULL;
		zend_string *func_name;
		zend_function *current_mptr;
		zend_bool register_shutdown = 1;

		if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|b", &obj, php_session_iface_entry, &register_shutdown) == FAILURE) {
			RETURN_FALSE;
		}

		/* For compatibility reason, implemeted interface is not checked */
		/* Find implemented methods - SessionHandlerInterface */
		i = 0;
		ZEND_HASH_FOREACH_STR_KEY(&php_session_iface_entry->function_table, func_name) {
			if ((current_mptr = zend_hash_find_ptr(&Z_OBJCE_P(obj)->function_table, func_name))) {
				if (!Z_ISUNDEF(PS(mod_user_names).names[i])) {
					zval_ptr_dtor(&PS(mod_user_names).names[i]);
				}

				array_init_size(&PS(mod_user_names).names[i], 2);
				Z_ADDREF_P(obj);
				add_next_index_zval(&PS(mod_user_names).names[i], obj);
				add_next_index_str(&PS(mod_user_names).names[i], zend_string_copy(func_name));
			} else {
				php_error_docref(NULL, E_ERROR, "Session handler's function table is corrupt");
				RETURN_FALSE;
			}

			++i;
		} ZEND_HASH_FOREACH_END();

		/* Find implemented methods - SessionIdInterface (optional) */
		ZEND_HASH_FOREACH_STR_KEY(&php_session_id_iface_entry->function_table, func_name) {
			if ((current_mptr = zend_hash_find_ptr(&Z_OBJCE_P(obj)->function_table, func_name))) {
				if (!Z_ISUNDEF(PS(mod_user_names).names[i])) {
					zval_ptr_dtor(&PS(mod_user_names).names[i]);
				}
				array_init_size(&PS(mod_user_names).names[i], 2);
				Z_ADDREF_P(obj);
				add_next_index_zval(&PS(mod_user_names).names[i], obj);
				add_next_index_str(&PS(mod_user_names).names[i], zend_string_copy(func_name));
			} else {
				if (!Z_ISUNDEF(PS(mod_user_names).names[i])) {
					zval_ptr_dtor(&PS(mod_user_names).names[i]);
					ZVAL_UNDEF(&PS(mod_user_names).names[i]);
				}
			}

			++i;
		} ZEND_HASH_FOREACH_END();

		/* Find implemented methods - SessionUpdateTimestampInterface (optional) */
		ZEND_HASH_FOREACH_STR_KEY(&php_session_update_timestamp_iface_entry->function_table, func_name) {
			if ((current_mptr = zend_hash_find_ptr(&Z_OBJCE_P(obj)->function_table, func_name))) {
				if (!Z_ISUNDEF(PS(mod_user_names).names[i])) {
					zval_ptr_dtor(&PS(mod_user_names).names[i]);
				}
				array_init_size(&PS(mod_user_names).names[i], 2);
				Z_ADDREF_P(obj);
				add_next_index_zval(&PS(mod_user_names).names[i], obj);
				add_next_index_str(&PS(mod_user_names).names[i], zend_string_copy(func_name));
			} else {
				if (!Z_ISUNDEF(PS(mod_user_names).names[i])) {
					zval_ptr_dtor(&PS(mod_user_names).names[i]);
					ZVAL_UNDEF(&PS(mod_user_names).names[i]);
				}
			}
			++i;
		} ZEND_HASH_FOREACH_END();

		if (register_shutdown) {
			/* create shutdown function */
			php_shutdown_function_entry shutdown_function_entry;
			shutdown_function_entry.arg_count = 1;
			shutdown_function_entry.arguments = (zval *) safe_emalloc(sizeof(zval), 1, 0);

			ZVAL_STRING(&shutdown_function_entry.arguments[0], "session_register_shutdown");

			/* add shutdown function, removing the old one if it exists */
			if (!register_user_shutdown_function("session_shutdown", sizeof("session_shutdown") - 1, &shutdown_function_entry)) {
				zval_ptr_dtor(&shutdown_function_entry.arguments[0]);
				efree(shutdown_function_entry.arguments);
				php_error_docref(NULL, E_WARNING, "Unable to register session shutdown function");
				RETURN_FALSE;
			}
		} else {
			/* remove shutdown function */
			remove_user_shutdown_function("session_shutdown", sizeof("session_shutdown") - 1);
		}

		if (PS(mod) && PS(session_status) != php_session_active && PS(mod) != &ps_mod_user) {
			ini_name = zend_string_init("session.save_handler", sizeof("session.save_handler") - 1, 0);
			ini_val = zend_string_init("user", sizeof("user") - 1, 0);
			PS(set_handler) = 1;
			zend_alter_ini_entry(ini_name, ini_val, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
			PS(set_handler) = 0;
			zend_string_release(ini_val);
			zend_string_release(ini_name);
		}

		RETURN_TRUE;
	}

	/* Set procedural save handler functions */
	if (argc < 6 || PS_NUM_APIS < argc) {
		WRONG_PARAM_COUNT;
	}

	if (zend_parse_parameters(argc, "+", &args, &num_args) == FAILURE) {
		return;
	}

	/* remove shutdown function */
	remove_user_shutdown_function("session_shutdown", sizeof("session_shutdown") - 1);

	/* At this point argc can only be between 6 and PS_NUM_APIS */
	for (i = 0; i < argc; i++) {
		if (!zend_is_callable(&args[i], 0, NULL)) {
			zend_string *name = zend_get_callable_name(&args[i]);
			php_error_docref(NULL, E_WARNING, "Argument %d is not a valid callback", i+1);
			zend_string_release(name);
			RETURN_FALSE;
		}
	}

	if (PS(mod) && PS(mod) != &ps_mod_user) {
		ini_name = zend_string_init("session.save_handler", sizeof("session.save_handler") - 1, 0);
		ini_val = zend_string_init("user", sizeof("user") - 1, 0);
		PS(set_handler) = 1;
		zend_alter_ini_entry(ini_name, ini_val, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
		PS(set_handler) = 0;
		zend_string_release(ini_val);
		zend_string_release(ini_name);
	}

	for (i = 0; i < argc; i++) {
		if (!Z_ISUNDEF(PS(mod_user_names).names[i])) {
			zval_ptr_dtor(&PS(mod_user_names).names[i]);
		}
		ZVAL_COPY(&PS(mod_user_names).names[i], &args[i]);
	}

	RETURN_TRUE;
}