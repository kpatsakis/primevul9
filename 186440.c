int php_libxml_register_export(zend_class_entry *ce, php_libxml_export_node export_function)
{
	php_libxml_func_handler export_hnd;
	
	/* Initialize in case this module hasnt been loaded yet */
	php_libxml_initialize();
	export_hnd.export_func = export_function;

	return zend_hash_add(&php_libxml_exports, ce->name, ce->name_length + 1, &export_hnd, sizeof(export_hnd), NULL);
}