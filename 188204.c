
PHPAPI void php_info_print_hr(void) /* {{{ */
{
	if (!sapi_module.phpinfo_as_text) {
		php_info_print("<hr />\n");
	} else {
		php_info_print("\n\n _______________________________________________________________________\n\n");
	}