encodePtr get_conversion(int encode)
{
	encodePtr *enc = NULL;
	TSRMLS_FETCH();

	if (zend_hash_index_find(&SOAP_GLOBAL(defEncIndex), encode, (void **)&enc) == FAILURE) {
		soap_error0(E_ERROR,  "Encoding: Cannot find encoding");
		return NULL;
	} else {
		return *enc;
	}
}