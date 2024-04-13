ZEND_API int zend_register_class_alias_ex(const char *name, int name_len, zend_class_entry *ce TSRMLS_DC) /* {{{ */
{
	char *lcname = zend_str_tolower_dup(name, name_len);
	int ret;

	if (lcname[0] == '\\') {
		ret = zend_hash_add(CG(class_table), lcname+1, name_len, &ce, sizeof(zend_class_entry *), NULL);
	} else {
		ret = zend_hash_add(CG(class_table), lcname, name_len+1, &ce, sizeof(zend_class_entry *), NULL);
	}

	efree(lcname);
	if (ret == SUCCESS) {
		ce->refcount++;
	}
	return ret;
}