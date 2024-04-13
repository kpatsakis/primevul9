static inline int unserialize_allowed_class(zend_string *class_name, HashTable *classes)
{
	zend_string *lcname;
	int res;
	ALLOCA_FLAG(use_heap)

	if(classes == NULL) {
		return 1;
	}
	if(!zend_hash_num_elements(classes)) {
		return 0;
	}

	STR_ALLOCA_ALLOC(lcname, class_name->len, use_heap);
	zend_str_tolower_copy(lcname->val, class_name->val, class_name->len);
	res = zend_hash_exists(classes, lcname);
	STR_ALLOCA_FREE(lcname, use_heap);
	return res;
}