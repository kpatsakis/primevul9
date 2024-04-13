static zend_object_value spl_filesystem_object_new_check(zend_class_entry *class_type TSRMLS_DC)
{
	zend_object_value ret = spl_filesystem_object_new_ex(class_type, NULL TSRMLS_CC);
	ret.handlers = &spl_filesystem_object_check_handlers;
	return ret;
}