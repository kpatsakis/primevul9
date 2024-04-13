 */
void php_info_print_style(TSRMLS_D)
{
	php_info_printf("<style type=\"text/css\">\n");
	php_info_print_css(TSRMLS_C);
	php_info_printf("</style>\n");