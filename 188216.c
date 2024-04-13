
PHPAPI void php_info_print_table_colspan_header(int num_cols, char *header) /* {{{ */
{
	int spaces;

	if (!sapi_module.phpinfo_as_text) {
		php_info_printf("<tr class=\"h\"><th colspan=\"%d\">%s</th></tr>\n", num_cols, header );
	} else {
		spaces = (74 - strlen(header));
		php_info_printf("%*s%s%*s\n", (int)(spaces/2), " ", header, (int)(spaces/2), " ");
	}