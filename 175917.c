void encode_reset_ns()
{
	TSRMLS_FETCH();
	SOAP_GLOBAL(cur_uniq_ns) = 0;
	SOAP_GLOBAL(cur_uniq_ref) = 0;
	if (SOAP_GLOBAL(ref_map)) {
		zend_hash_destroy(SOAP_GLOBAL(ref_map));
	} else {
		SOAP_GLOBAL(ref_map) = emalloc(sizeof(HashTable));
	}
	zend_hash_init(SOAP_GLOBAL(ref_map), 0, NULL, NULL, 0);
}