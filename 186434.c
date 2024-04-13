static PHP_FUNCTION(libxml_get_errors)
{
	
	xmlErrorPtr error;

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	if (LIBXML(error_list)) {

		error = zend_llist_get_first(LIBXML(error_list));

		while (error != NULL) {
			zval *z_error;
			MAKE_STD_ZVAL(z_error);

			object_init_ex(z_error, libxmlerror_class_entry);
			add_property_long(z_error, "level", error->level);
			add_property_long(z_error, "code", error->code);
			add_property_long(z_error, "column", error->int2);
			if (error->message) {
				add_property_string(z_error, "message", error->message, 1);
			} else {
				add_property_stringl(z_error, "message", "", 0, 1);
			}
			if (error->file) {
				add_property_string(z_error, "file", error->file, 1);
			} else {
				add_property_stringl(z_error, "file", "", 0, 1);
			}
			add_property_long(z_error, "line", error->line);
			add_next_index_zval(return_value, z_error);

			error = zend_llist_get_next(LIBXML(error_list));
		}
	}
}