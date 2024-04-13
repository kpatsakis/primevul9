ZEND_API int zend_disable_class(char *class_name, uint class_name_length TSRMLS_DC) /* {{{ */
{
	zend_class_entry **disabled_class;

	zend_str_tolower(class_name, class_name_length);
	if (zend_hash_find(CG(class_table), class_name, class_name_length+1, (void **)&disabled_class)==FAILURE) {
		return FAILURE;
	}
	INIT_CLASS_ENTRY_INIT_METHODS((**disabled_class), disabled_class_new, NULL, NULL, NULL, NULL, NULL);
	(*disabled_class)->create_object = display_disabled_class;
	zend_hash_clean(&((*disabled_class)->function_table));
	return SUCCESS;
}