static int _php_libxml_free_error(xmlErrorPtr error)
{
	/* This will free the libxml alloc'd memory */
	xmlResetError(error);
	return 1;
}