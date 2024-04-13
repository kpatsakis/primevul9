static PHP_GINIT_FUNCTION(ps) /* {{{ */
{
	int i;

#if defined(COMPILE_DL_SESSION) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	ps_globals->save_path = NULL;
	ps_globals->session_name = NULL;
	ps_globals->id = NULL;
	ps_globals->mod = NULL;
	ps_globals->serializer = NULL;
	ps_globals->mod_data = NULL;
	ps_globals->session_status = php_session_none;
	ps_globals->default_mod = NULL;
	ps_globals->mod_user_implemented = 0;
	ps_globals->mod_user_is_open = 0;
	ps_globals->session_vars = NULL;
	ps_globals->set_handler = 0;
	for (i = 0; i < PS_NUM_APIS; i++) {
		ZVAL_UNDEF(&ps_globals->mod_user_names.names[i]);
	}
	ZVAL_UNDEF(&ps_globals->http_session_vars);
}