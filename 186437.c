void php_libxml_issue_error(int level, const char *msg TSRMLS_DC)
{
	if (LIBXML(error_list)) {
		_php_list_set_error_structure(NULL, msg);
	} else {
		php_error_docref(NULL TSRMLS_CC, level, "%s", msg);
	}
}