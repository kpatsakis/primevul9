PHPAPI int php_session_register_module(ps_module *ptr) /* {{{ */
{
	int ret = FAILURE;
	int i;

	for (i = 0; i < MAX_MODULES; i++) {
		if (!ps_modules[i]) {
			ps_modules[i] = ptr;
			ret = SUCCESS;
			break;
		}
	}
	return ret;
}