
PHPAPI void php_info_print_box_start(int flag) /* {{{ */
{
	php_info_print_table_start();
	if (flag) {
		if (!sapi_module.phpinfo_as_text) {
			php_info_print("<tr class=\"h\"><td>\n");
		}
	} else {
		if (!sapi_module.phpinfo_as_text) {
			php_info_print("<tr class=\"v\"><td>\n");
		} else {
			php_info_print("\n");
		}
	}