static HashTable *php_snmp_get_gc(zval *object, zval ***gc_data, int *gc_data_count TSRMLS_DC) /* {{{ */
{
	*gc_data = NULL;
	*gc_data_count = 0;
	return zend_std_get_properties(object TSRMLS_CC);
}