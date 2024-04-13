static PHP_MINFO_FUNCTION(libxml)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "libXML support", "active");
	php_info_print_table_row(2, "libXML Compiled Version", LIBXML_DOTTED_VERSION);
	php_info_print_table_row(2, "libXML Loaded Version", (char *)xmlParserVersion);
	php_info_print_table_row(2, "libXML streams", "enabled");
	php_info_print_table_end();
}