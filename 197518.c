static zend_class_entry *do_register_internal_class(zend_class_entry *orig_class_entry, zend_uint ce_flags TSRMLS_DC) /* {{{ */
{
	zend_class_entry *class_entry = malloc(sizeof(zend_class_entry));
	char *lowercase_name = emalloc(orig_class_entry->name_length + 1);
	zend_ulong hash;
	*class_entry = *orig_class_entry;

	class_entry->type = ZEND_INTERNAL_CLASS;
	zend_initialize_class_data(class_entry, 0 TSRMLS_CC);
	class_entry->ce_flags = ce_flags;
	class_entry->info.internal.module = EG(current_module);

	if (class_entry->info.internal.builtin_functions) {
		zend_register_functions(class_entry, class_entry->info.internal.builtin_functions, &class_entry->function_table, MODULE_PERSISTENT TSRMLS_CC);
	}

	zend_str_tolower_copy(lowercase_name, orig_class_entry->name, class_entry->name_length);
	lowercase_name = (char*)zend_new_interned_string(lowercase_name, class_entry->name_length + 1, 1 TSRMLS_CC);
	hash = str_hash(lowercase_name, class_entry->name_length);
	zend_hash_quick_update(CG(class_table), lowercase_name, class_entry->name_length+1, hash, &class_entry, sizeof(zend_class_entry *), NULL);
	str_efree(lowercase_name);
	return class_entry;
}