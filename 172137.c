PHP_MINFO_FUNCTION(filter)
{
	php_info_print_table_start();
	php_info_print_table_row( 2, "Input Validation and Filtering", "enabled" );
	php_info_print_table_row( 2, "Revision", "$Id$");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}