PHP_METHOD(snmp, setSecurity)
{
	php_snmp_object *snmp_object;
	zval *object = getThis();
	char *a1 = "", *a2 = "", *a3 = "", *a4 = "", *a5 = "", *a6 = "", *a7 = "";
	int a1_len = 0, a2_len = 0, a3_len = 0, a4_len = 0, a5_len = 0, a6_len = 0, a7_len = 0;
	int argc = ZEND_NUM_ARGS();

	snmp_object = (php_snmp_object *)zend_object_store_get_object(object TSRMLS_CC);

	if (zend_parse_parameters(argc TSRMLS_CC, "s|ssssss", &a1, &a1_len, &a2, &a2_len, &a3, &a3_len,
		&a4, &a4_len, &a5, &a5_len, &a6, &a6_len, &a7, &a7_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (netsnmp_session_set_security(snmp_object->session, a1, a2, a3, a4, a5, a6, a7 TSRMLS_CC)) {
		/* Warning message sent already, just bail out */
		RETURN_FALSE;
	}
	RETURN_TRUE;
}