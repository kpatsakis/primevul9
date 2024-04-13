PHP_LIBXML_API zend_bool php_libxml_disable_entity_loader(zend_bool disable TSRMLS_DC)
{
	zend_bool old = LIBXML(entity_loader_disabled);

	LIBXML(entity_loader_disabled) = disable;
	return old;
}