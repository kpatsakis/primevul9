PHP_LIBXML_API void php_libxml_shutdown(void)
{
	if (_php_libxml_initialized) {
#if defined(LIBXML_SCHEMAS_ENABLED)
		xmlRelaxNGCleanupTypes();
#endif
		xmlCleanupParser();
		zend_hash_destroy(&php_libxml_exports);
		
		xmlSetExternalEntityLoader(_php_libxml_default_entity_loader);
		_php_libxml_initialized = 0;
	}
}