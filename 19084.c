PHP_FUNCTION(snmp_set_valueretrieval)
{
	long method;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &method) == FAILURE) {
		RETURN_FALSE;
	}

	if (method >= 0 && method <= (SNMP_VALUE_LIBRARY|SNMP_VALUE_PLAIN|SNMP_VALUE_OBJECT)) {
			SNMP_G(valueretrieval) = method;
			RETURN_TRUE;
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown SNMP value retrieval method '%ld'", method);
		RETURN_FALSE;
	}
}