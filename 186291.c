static PHP_INI_MH(OnUpdateSaveHandler) /* {{{ */
{
	ps_module *tmp;

	SESSION_CHECK_ACTIVE_STATE;
	SESSION_CHECK_OUTPUT_STATE;

	tmp = _php_find_ps_module(ZSTR_VAL(new_value));

	if (PG(modules_activated) && !tmp) {
		int err_type;

		if (stage == ZEND_INI_STAGE_RUNTIME) {
			err_type = E_WARNING;
		} else {
			err_type = E_ERROR;
		}

		/* Do not output error when restoring ini options. */
		if (stage != ZEND_INI_STAGE_DEACTIVATE) {
			php_error_docref(NULL, err_type, "Cannot find save handler '%s'", ZSTR_VAL(new_value));
		}

		return FAILURE;
	}

	/* "user" save handler should not be set by user */
	if (!PS(set_handler) &&  tmp == ps_user_ptr) {
		php_error_docref(NULL, E_RECOVERABLE_ERROR, "Cannot set 'user' save handler by ini_set() or session_module_name()");
		return FAILURE;
	}

	PS(default_mod) = PS(mod);
	PS(mod) = tmp;

	return SUCCESS;
}