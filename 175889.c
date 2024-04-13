static zend_bool soap_check_xml_ref(zval **data, xmlNodePtr node TSRMLS_DC)
{
	zval **data_ptr;

	if (SOAP_GLOBAL(ref_map)) {
		if (zend_hash_index_find(SOAP_GLOBAL(ref_map), (ulong)node, (void**)&data_ptr) == SUCCESS) {
			if (*data != *data_ptr) {
				zval_ptr_dtor(data);
				*data = *data_ptr;
				Z_SET_ISREF_PP(data);
				Z_ADDREF_PP(data);
				return 1;
			}
		} else {
			zend_hash_index_update(SOAP_GLOBAL(ref_map), (ulong)node, (void**)data, sizeof(zval*), NULL);
		}
	}
	return 0;
}