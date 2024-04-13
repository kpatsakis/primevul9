 */
PHPAPI char *php_info_html_esc(char *string TSRMLS_DC)
{
	size_t new_len;
	return php_escape_html_entities((unsigned char *) string, strlen(string), &new_len, 0, ENT_QUOTES, NULL TSRMLS_CC);