static zend_object_value spl_filesystem_object_new_ex(zend_class_entry *class_type, spl_filesystem_object **obj TSRMLS_DC)
{
	zend_object_value retval;
	spl_filesystem_object *intern;

	intern = emalloc(sizeof(spl_filesystem_object));
	memset(intern, 0, sizeof(spl_filesystem_object));
	/* intern->type = SPL_FS_INFO; done by set 0 */
	intern->file_class = spl_ce_SplFileObject;
	intern->info_class = spl_ce_SplFileInfo;
	if (obj) *obj = intern;

	zend_object_std_init(&intern->std, class_type TSRMLS_CC);
	object_properties_init(&intern->std, class_type);

	retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t) zend_objects_destroy_object, (zend_objects_free_object_storage_t) spl_filesystem_object_free_storage, NULL TSRMLS_CC);
	retval.handlers = &spl_filesystem_object_handlers;
	return retval;
}