static PHP_INI_MH(OnUpdateSerializer) /* {{{ */
{
	const ps_serializer *tmp;

	SESSION_CHECK_ACTIVE_STATE;
	SESSION_CHECK_OUTPUT_STATE;

	tmp = _php_find_ps_serializer(ZSTR_VAL(new_value));

	if (PG(modules_activated) && !tmp) {
		int err_type;

		if (stage == ZEND_INI_STAGE_RUNTIME) {
			err_type = E_WARNING;
		} else {
			err_type = E_ERROR;
		}

		/* Do not output error when restoring ini options. */
		if (stage != ZEND_INI_STAGE_DEACTIVATE) {
			php_error_docref(NULL, err_type, "Cannot find serialization handler '%s'", ZSTR_VAL(new_value));
		}
		return FAILURE;
	}
	PS(serializer) = tmp;

	return SUCCESS;
}