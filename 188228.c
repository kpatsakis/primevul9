 */
static void php_info_print_table_row_internal(int num_cols,
		const char *value_class, va_list row_elements)
{
	int i;
	char *row_element;

	if (!sapi_module.phpinfo_as_text) {
		php_info_print("<tr>");
	}
	for (i=0; i<num_cols; i++) {
		if (!sapi_module.phpinfo_as_text) {
			php_info_printf("<td class=\"%s\">",
			   (i==0 ? "e" : value_class )
			);
		}
		row_element = va_arg(row_elements, char *);
		if (!row_element || !*row_element) {
			if (!sapi_module.phpinfo_as_text) {
				php_info_print( "<i>no value</i>" );
			} else {
				php_info_print( " " );
			}
		} else {
			if (!sapi_module.phpinfo_as_text) {
				php_info_print_html_esc(row_element, strlen(row_element));
			} else {
				php_info_print(row_element);
				if (i < num_cols-1) {
					php_info_print(" => ");
				}
			}
		}
		if (!sapi_module.phpinfo_as_text) {
			php_info_print(" </td>");
		} else if (i == (num_cols - 1)) {
			php_info_print("\n");
		}
	}
	if (!sapi_module.phpinfo_as_text) {
		php_info_print("</tr>\n");
	}