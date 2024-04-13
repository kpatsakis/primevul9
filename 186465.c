static void _php_list_set_error_structure(xmlErrorPtr error, const char *msg)
{
	xmlError error_copy;
	int ret;

	TSRMLS_FETCH();

	memset(&error_copy, 0, sizeof(xmlError));

	if (error) {
		ret = xmlCopyError(error, &error_copy);
	} else {
		error_copy.domain = 0;
		error_copy.code = XML_ERR_INTERNAL_ERROR;
		error_copy.level = XML_ERR_ERROR;
		error_copy.line = 0;
		error_copy.node = NULL;
		error_copy.int1 = 0;
		error_copy.int2 = 0;
		error_copy.ctxt = NULL;
		error_copy.message = xmlStrdup(msg);
		error_copy.file = NULL;
		error_copy.str1 = NULL;
		error_copy.str2 = NULL;
		error_copy.str3 = NULL;
		ret = 0;
	}

	if (ret == 0) {
		zend_llist_add_element(LIBXML(error_list), &error_copy);
	}
}