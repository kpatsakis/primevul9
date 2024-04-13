static inline void php_rinit_session_globals(void) /* {{{ */
{
	/* Do NOT init PS(mod_user_names) here! */
	/* TODO: These could be moved to MINIT and removed. These should be initialized by php_rshutdown_session_globals() always when execution is finished. */
	PS(id) = NULL;
	PS(session_status) = php_session_none;
	PS(in_save_handler) = 0;
	PS(set_handler) = 0;
	PS(mod_data) = NULL;
	PS(mod_user_is_open) = 0;
	PS(define_sid) = 1;
	PS(session_vars) = NULL;
	PS(module_number) = my_module_number;
	ZVAL_UNDEF(&PS(http_session_vars));
}