static PHP_FUNCTION(libxml_disable_entity_loader)
{
	zend_bool disable = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &disable) == FAILURE) {
		return;
	}

	RETURN_BOOL(php_libxml_disable_entity_loader(disable TSRMLS_CC));
}