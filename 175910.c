void encode_finish()
{
	TSRMLS_FETCH();
	SOAP_GLOBAL(cur_uniq_ns) = 0;
	SOAP_GLOBAL(cur_uniq_ref) = 0;
	if (SOAP_GLOBAL(ref_map)) {
		zend_hash_destroy(SOAP_GLOBAL(ref_map));
		efree(SOAP_GLOBAL(ref_map));
		SOAP_GLOBAL(ref_map) = NULL;
	}
}