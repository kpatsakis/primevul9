PHP_MINFO_FUNCTION(miconv)
{
	zval iconv_impl, iconv_ver;

	zend_get_constant("ICONV_IMPL", sizeof("ICONV_IMPL")-1, &iconv_impl TSRMLS_CC);
	zend_get_constant("ICONV_VERSION", sizeof("ICONV_VERSION")-1, &iconv_ver TSRMLS_CC);

	php_info_print_table_start();
	php_info_print_table_row(2, "iconv support", "enabled");
	php_info_print_table_row(2, "iconv implementation", Z_STRVAL(iconv_impl));
	php_info_print_table_row(2, "iconv library version", Z_STRVAL(iconv_ver));
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();

	zval_dtor(&iconv_impl);
	zval_dtor(&iconv_ver);
}