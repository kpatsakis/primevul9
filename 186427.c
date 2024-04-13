static PHP_FUNCTION(libxml_get_last_error)
{
	xmlErrorPtr error;

	error = xmlGetLastError();
	
	if (error) {
		object_init_ex(return_value, libxmlerror_class_entry);
		add_property_long(return_value, "level", error->level);
		add_property_long(return_value, "code", error->code);
		add_property_long(return_value, "column", error->int2);
		if (error->message) {
			add_property_string(return_value, "message", error->message, 1);
		} else {
			add_property_stringl(return_value, "message", "", 0, 1);
		}
		if (error->file) {
			add_property_string(return_value, "file", error->file, 1);
		} else {
			add_property_stringl(return_value, "file", "", 0, 1);
		}
		add_property_long(return_value, "line", error->line);
	} else {
		RETURN_FALSE;
	}
}