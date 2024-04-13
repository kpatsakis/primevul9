 */
PHP_MINFO_FUNCTION(imap)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "IMAP c-Client Version", CCLIENTVERSION);
#if HAVE_IMAP_SSL
	php_info_print_table_row(2, "SSL Support", "enabled");
#endif
#if HAVE_IMAP_KRB && HAVE_IMAP_AUTH_GSS
	php_info_print_table_row(2, "Kerberos Support", "enabled");
#endif
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();