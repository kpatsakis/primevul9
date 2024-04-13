static PHP_FUNCTION(libxml_set_external_entity_loader)
{
	zend_fcall_info			fci;
	zend_fcall_info_cache	fcc;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "f!", &fci, &fcc)
			== FAILURE) {
		return;
	}
	
	_php_libxml_destroy_fci(&LIBXML(entity_loader).fci);
	
	if (fci.size > 0) { /* argument not null */
		LIBXML(entity_loader).fci = fci;
		Z_ADDREF_P(fci.function_name);
		if (fci.object_ptr != NULL) {
			Z_ADDREF_P(fci.object_ptr);
		}
		LIBXML(entity_loader).fcc = fcc;
	}
	
	RETURN_TRUE;
}