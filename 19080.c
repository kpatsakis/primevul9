PHP_METHOD(snmp, __construct)
{
	php_snmp_object *snmp_object;
	zval *object = getThis();
	char *a1, *a2;
	int a1_len, a2_len;
	long timeout = SNMP_DEFAULT_TIMEOUT;
	long retries = SNMP_DEFAULT_RETRIES;
	long version = SNMP_DEFAULT_VERSION;
	int argc = ZEND_NUM_ARGS();
	zend_error_handling error_handling;

	snmp_object = (php_snmp_object *)zend_object_store_get_object(object TSRMLS_CC);
	zend_replace_error_handling(EH_THROW, NULL, &error_handling TSRMLS_CC);

	if (zend_parse_parameters(argc TSRMLS_CC, "lss|ll", &version, &a1, &a1_len, &a2, &a2_len, &timeout, &retries) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}

	zend_restore_error_handling(&error_handling TSRMLS_CC);

	switch(version) {
		case SNMP_VERSION_1:
		case SNMP_VERSION_2c:
		case SNMP_VERSION_3:
			break;
		default:
			zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Unknown SNMP protocol version", 0 TSRMLS_CC);
			return;
	}

	/* handle re-open of snmp session */
	if (snmp_object->session) {
		netsnmp_session_free(&(snmp_object->session));
	}

	if (netsnmp_session_init(&(snmp_object->session), version, a1, a2, timeout, retries TSRMLS_CC)) {
		return;
	}
	snmp_object->max_oids = 0;
	snmp_object->valueretrieval = SNMP_G(valueretrieval);
	snmp_object->enum_print = netsnmp_ds_get_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_PRINT_NUMERIC_ENUM);
	snmp_object->oid_output_format = netsnmp_ds_get_int(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_OID_OUTPUT_FORMAT);
	snmp_object->quick_print = netsnmp_ds_get_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_QUICK_PRINT);
	snmp_object->oid_increasing_check = TRUE;
	snmp_object->exceptions_enabled = 0;
}