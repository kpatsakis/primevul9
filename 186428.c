static PHP_FUNCTION(libxml_clear_errors)
{
	xmlResetLastError();
	if (LIBXML(error_list)) {
		zend_llist_clean(LIBXML(error_list));
	}
}