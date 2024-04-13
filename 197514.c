ZEND_API const char* zend_resolve_method_name(zend_class_entry *ce, zend_function *f) /* {{{ */
{
	zend_function *func;
	HashPosition iterator;
	HashTable *function_table;

	if (f->common.type != ZEND_USER_FUNCTION ||
	    *(f->op_array.refcount) < 2 ||
	    !f->common.scope ||
	    !f->common.scope->trait_aliases) {
		return f->common.function_name;
	}

	function_table = &ce->function_table;
	zend_hash_internal_pointer_reset_ex(function_table, &iterator);
	while (zend_hash_get_current_data_ex(function_table, (void **)&func, &iterator) == SUCCESS) {
		if (func == f) {
			char *name;
			uint len;
			ulong idx;

			if (zend_hash_get_current_key_ex(function_table, &name, &len, &idx, 0, &iterator) != HASH_KEY_IS_STRING) {
				return f->common.function_name;
			}
			--len;
			if (len == strlen(f->common.function_name) &&
			    !strncasecmp(name, f->common.function_name, len)) {
				return f->common.function_name;
			}
			return zend_find_alias_name(f->common.scope, name, len);
		}
		zend_hash_move_forward_ex(function_table, &iterator);
	}
	return f->common.function_name;
}