 */
PHPAPI void php_info_print_table_header(int num_cols, ...)
{
	int i;
	va_list row_elements;
	char *row_element;

	va_start(row_elements, num_cols);
	if (!sapi_module.phpinfo_as_text) {
		php_info_print("<tr class=\"h\">");
	}
	for (i=0; i<num_cols; i++) {
		row_element = va_arg(row_elements, char *);
		if (!row_element || !*row_element) {
			row_element = " ";
		}
		if (!sapi_module.phpinfo_as_text) {
			php_info_print("<th>");
			php_info_print(row_element);
			php_info_print("</th>");
		} else {
			php_info_print(row_element);
			if (i < num_cols-1) {
				php_info_print(" => ");
			} else {
				php_info_print("\n");
			}
		}
	}
	if (!sapi_module.phpinfo_as_text) {
		php_info_print("</tr>\n");
	}

	va_end(row_elements);