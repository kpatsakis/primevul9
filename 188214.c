
PHPAPI void php_info_print_module(zend_module_entry *zend_module TSRMLS_DC) /* {{{ */
{
	if (zend_module->info_func || zend_module->version) {
		if (!sapi_module.phpinfo_as_text) {
			php_info_printf("<h2><a name=\"module_%s\">%s</a></h2>\n", zend_module->name, zend_module->name);
		} else {
			php_info_print_table_start();
			php_info_print_table_header(1, zend_module->name);
			php_info_print_table_end();
		}
		if (zend_module->info_func) {
			zend_module->info_func(zend_module TSRMLS_CC);
		} else {
			php_info_print_table_start();
			php_info_print_table_row(2, "Version", zend_module->version);
			php_info_print_table_end();
			DISPLAY_INI_ENTRIES();
		}
	} else {
		if (!sapi_module.phpinfo_as_text) {
			php_info_printf("<tr><td class=\"v\">%s</td></tr>\n", zend_module->name);
		} else {
			php_info_printf("%s\n", zend_module->name);
		}
	}