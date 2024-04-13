ZEND_API void zend_merge_properties(zval *obj, HashTable *properties, int destroy_ht TSRMLS_DC) /* {{{ */
{
	const zend_object_handlers *obj_ht = Z_OBJ_HT_P(obj);
	zend_class_entry *old_scope = EG(scope);

	EG(scope) = Z_OBJCE_P(obj);
	zend_hash_apply_with_arguments(properties TSRMLS_CC, (apply_func_args_t)zend_merge_property, 2, obj, obj_ht);
	EG(scope) = old_scope;

	if (destroy_ht) {
		zend_hash_destroy(properties);
		FREE_HASHTABLE(properties);
	}
}