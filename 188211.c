 */
PHPAPI void php_info_print_table_row_ex(int num_cols, const char *value_class,
		...)
{
	va_list row_elements;

	va_start(row_elements, value_class);
	php_info_print_table_row_internal(num_cols, value_class, row_elements);
	va_end(row_elements);