static void spl_filesystem_dir_it_current_key(zend_object_iterator *iter, zval *key TSRMLS_DC)
{
	spl_filesystem_object *object = spl_filesystem_iterator_to_object((spl_filesystem_iterator *)iter);

	ZVAL_LONG(key, object->u.dir.index);
}