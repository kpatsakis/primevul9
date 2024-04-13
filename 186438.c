PHP_LIBXML_API void php_libxml_initialize(void)
{
	if (!_php_libxml_initialized) {
		/* we should be the only one's to ever init!! */
		xmlInitParser();
		
		_php_libxml_default_entity_loader = xmlGetExternalEntityLoader();
		xmlSetExternalEntityLoader(_php_libxml_pre_ext_ent_loader);

		zend_hash_init(&php_libxml_exports, 0, NULL, NULL, 1);

		_php_libxml_initialized = 1;
	}
}