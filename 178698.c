static void spl_filesystem_tree_it_current_key(zend_object_iterator *iter, zval *key TSRMLS_DC)
{
	spl_filesystem_object *object = spl_filesystem_iterator_to_object((spl_filesystem_iterator *)iter);

	if (SPL_FILE_DIR_KEY(object, SPL_FILE_DIR_KEY_AS_FILENAME)) {
		ZVAL_STRING(key, object->u.dir.entry.d_name, 1);
	} else {
		spl_filesystem_object_get_file_name(object TSRMLS_CC);
		ZVAL_STRINGL(key, object->file_name, object->file_name_len, 1);
	}
}