
PHPAPI void php_info_print_table_start(void) /* {{{ */
{
	if (!sapi_module.phpinfo_as_text) {
		php_info_print("<table>\n");
	} else {
		php_info_print("\n");
	}