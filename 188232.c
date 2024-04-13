 */
PHPAPI void php_info_print_table_row(int num_cols, ...)
{
	va_list row_elements;

	va_start(row_elements, num_cols);
	php_info_print_table_row_internal(num_cols, "v", row_elements);
	va_end(row_elements);