static void php_snmp_error(zval *object, const char *docref TSRMLS_DC, int type, const char *format, ...)
{
	va_list args;
	php_snmp_object *snmp_object = NULL;

	if (object) {
		snmp_object = (php_snmp_object *)zend_object_store_get_object(object TSRMLS_CC);
		if (type == PHP_SNMP_ERRNO_NOERROR) {
			memset(snmp_object->snmp_errstr, 0, sizeof(snmp_object->snmp_errstr));
		} else {
			va_start(args, format);
			vsnprintf(snmp_object->snmp_errstr, sizeof(snmp_object->snmp_errstr) - 1, format, args);
			va_end(args);
		}
		snmp_object->snmp_errno = type;
	}

	if (type == PHP_SNMP_ERRNO_NOERROR) {
		return;
	}

	if (object && (snmp_object->exceptions_enabled & type)) {
		zend_throw_exception_ex(php_snmp_exception_ce, type TSRMLS_CC, "%s", snmp_object->snmp_errstr);
	} else {
		va_start(args, format);
		php_verror(docref, "", E_WARNING, format, args TSRMLS_CC);
		va_end(args);
	}
}