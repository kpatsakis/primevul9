
PHPAPI void php_info_print_table_end(void) /* {{{ */
{
	if (!sapi_module.phpinfo_as_text) {
		php_info_print("</table>\n");
	}
