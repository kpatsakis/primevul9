static void _php_libxml_destroy_fci(zend_fcall_info *fci)
{
	if (fci->size > 0) {
		zval_ptr_dtor(&fci->function_name);
		if (fci->object_ptr != NULL) {
			zval_ptr_dtor(&fci->object_ptr);
		}
		fci->size = 0;
	}
}