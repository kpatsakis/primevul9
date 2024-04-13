static PHP_MINFO_FUNCTION(session) /* {{{ */
{
	ps_module **mod;
	ps_serializer *ser;
	smart_str save_handlers = {0};
	smart_str ser_handlers = {0};
	int i;

	/* Get save handlers */
	for (i = 0, mod = ps_modules; i < MAX_MODULES; i++, mod++) {
		if (*mod && (*mod)->s_name) {
			smart_str_appends(&save_handlers, (*mod)->s_name);
			smart_str_appendc(&save_handlers, ' ');
		}
	}

	/* Get serializer handlers */
	for (i = 0, ser = ps_serializers; i < MAX_SERIALIZERS; i++, ser++) {
		if (ser && ser->name) {
			smart_str_appends(&ser_handlers, ser->name);
			smart_str_appendc(&ser_handlers, ' ');
		}
	}

	php_info_print_table_start();
	php_info_print_table_row(2, "Session Support", "enabled" );

	if (save_handlers.s) {
		smart_str_0(&save_handlers);
		php_info_print_table_row(2, "Registered save handlers", ZSTR_VAL(save_handlers.s));
		smart_str_free(&save_handlers);
	} else {
		php_info_print_table_row(2, "Registered save handlers", "none");
	}

	if (ser_handlers.s) {
		smart_str_0(&ser_handlers);
		php_info_print_table_row(2, "Registered serializer handlers", ZSTR_VAL(ser_handlers.s));
		smart_str_free(&ser_handlers);
	} else {
		php_info_print_table_row(2, "Registered serializer handlers", "none");
	}

	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}