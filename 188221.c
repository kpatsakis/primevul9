
static int php_info_print_html_esc(const char *str, int len) /* {{{ */
{
	size_t new_len;
	int written;
	char *new_str;
	TSRMLS_FETCH();

	new_str = php_escape_html_entities((unsigned char *) str, len, &new_len, 0, ENT_QUOTES, "utf-8" TSRMLS_CC);
	written = php_output_write(new_str, new_len TSRMLS_CC);
	str_efree(new_str);
	return written;