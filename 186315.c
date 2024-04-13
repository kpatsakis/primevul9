static PHP_FUNCTION(session_register_shutdown)
{
	php_shutdown_function_entry shutdown_function_entry;

	/* This function is registered itself as a shutdown function by
	 * session_set_save_handler($obj). The reason we now register another
	 * shutdown function is in case the user registered their own shutdown
	 * function after calling session_set_save_handler(), which expects
	 * the session still to be available.
	 */

	shutdown_function_entry.arg_count = 1;
	shutdown_function_entry.arguments = (zval *) safe_emalloc(sizeof(zval), 1, 0);

	ZVAL_STRING(&shutdown_function_entry.arguments[0], "session_write_close");

	if (!append_user_shutdown_function(shutdown_function_entry)) {
		zval_ptr_dtor(&shutdown_function_entry.arguments[0]);
		efree(shutdown_function_entry.arguments);

		/* Unable to register shutdown function, presumably because of lack
		 * of memory, so flush the session now. It would be done in rshutdown
		 * anyway but the handler will have had it's dtor called by then.
		 * If the user does have a later shutdown function which needs the
		 * session then tough luck.
		 */
		php_session_flush(1);
		php_error_docref(NULL, E_WARNING, "Unable to register session flush function");
	}
}