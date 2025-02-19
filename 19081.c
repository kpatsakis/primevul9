PHP_FUNCTION(snmp_set_oid_output_format)
{
	long a1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &a1) == FAILURE) {
		RETURN_FALSE;
	}

	switch((int) a1) {
		case NETSNMP_OID_OUTPUT_SUFFIX:
		case NETSNMP_OID_OUTPUT_MODULE:
		case NETSNMP_OID_OUTPUT_FULL:
		case NETSNMP_OID_OUTPUT_NUMERIC:
		case NETSNMP_OID_OUTPUT_UCD:
		case NETSNMP_OID_OUTPUT_NONE:
			netsnmp_ds_set_int(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_OID_OUTPUT_FORMAT, a1);
			RETURN_TRUE;
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown SNMP output print format '%d'", (int) a1);
			RETURN_FALSE;
			break;
	}
}