ZEND_API void object_properties_init(zend_object *object, zend_class_entry *class_type) /* {{{ */
{
	int i;

	if (class_type->default_properties_count) {
		object->properties_table = emalloc(sizeof(zval*) * class_type->default_properties_count);
		for (i = 0; i < class_type->default_properties_count; i++) {
			object->properties_table[i] = class_type->default_properties_table[i];
			if (class_type->default_properties_table[i]) {
#if ZTS
				ALLOC_ZVAL( object->properties_table[i]);
				MAKE_COPY_ZVAL(&class_type->default_properties_table[i], object->properties_table[i]);
#else
				Z_ADDREF_P(object->properties_table[i]);
#endif
			}
		}
		object->properties = NULL;
	}
}