PHP_MINIT_FUNCTION(miconv)
{
	char *version = "unknown";

	REGISTER_INI_ENTRIES();

#if HAVE_LIBICONV
	{
		static char buf[16];
		snprintf(buf, sizeof(buf), "%d.%d",
		    ((_libiconv_version >> 8) & 0x0f), (_libiconv_version & 0x0f));
		version = buf;
	}
#elif HAVE_GLIBC_ICONV
	version = (char *)gnu_get_libc_version();
#elif defined(NETWARE)
	version = "OS built-in";
#endif

#ifdef PHP_ICONV_IMPL
	REGISTER_STRING_CONSTANT("ICONV_IMPL", PHP_ICONV_IMPL, CONST_CS | CONST_PERSISTENT);
#elif HAVE_LIBICONV
	REGISTER_STRING_CONSTANT("ICONV_IMPL", "libiconv", CONST_CS | CONST_PERSISTENT);
#elif defined(NETWARE)
	REGISTER_STRING_CONSTANT("ICONV_IMPL", "Novell", CONST_CS | CONST_PERSISTENT);
#else
	REGISTER_STRING_CONSTANT("ICONV_IMPL", "unknown", CONST_CS | CONST_PERSISTENT);
#endif
	REGISTER_STRING_CONSTANT("ICONV_VERSION", version, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("ICONV_MIME_DECODE_STRICT", PHP_ICONV_MIME_DECODE_STRICT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ICONV_MIME_DECODE_CONTINUE_ON_ERROR", PHP_ICONV_MIME_DECODE_CONTINUE_ON_ERROR, CONST_CS | CONST_PERSISTENT);

	if (php_iconv_stream_filter_register_factory(TSRMLS_C) != PHP_ICONV_ERR_SUCCESS) {
		return FAILURE;
	}

	php_output_handler_alias_register(ZEND_STRL("ob_iconv_handler"), php_iconv_output_handler_init TSRMLS_CC);
	php_output_handler_conflict_register(ZEND_STRL("ob_iconv_handler"), php_iconv_output_conflict TSRMLS_CC);

	return SUCCESS;
}