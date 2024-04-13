void curlfile_register_class(TSRMLS_D)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY( ce, "CURLFile", curlfile_funcs );
	curl_CURLFile_class = zend_register_internal_class(&ce TSRMLS_CC);
	zend_declare_property_string(curl_CURLFile_class, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_string(curl_CURLFile_class, "mime", sizeof("mime")-1, "", ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_string(curl_CURLFile_class, "postname", sizeof("postname")-1, "", ZEND_ACC_PUBLIC TSRMLS_CC);
}