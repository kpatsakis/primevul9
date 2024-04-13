static zval* soap_find_xml_ref(xmlNodePtr node TSRMLS_DC)
{
	zval **data_ptr;

	if (SOAP_GLOBAL(ref_map) && 
	    zend_hash_index_find(SOAP_GLOBAL(ref_map), (ulong)node, (void**)&data_ptr) == SUCCESS) {
		Z_SET_ISREF_PP(data_ptr);
		Z_ADDREF_PP(data_ptr);
		return *data_ptr;
	}
	return NULL;
}