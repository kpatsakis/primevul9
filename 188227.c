 */
PHPAPI void php_print_info_htmlhead(TSRMLS_D)
{
	php_info_print("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"DTD/xhtml1-transitional.dtd\">\n");
	php_info_print("<html xmlns=\"http://www.w3.org/1999/xhtml\">");
	php_info_print("<head>\n");
	php_info_print_style(TSRMLS_C);
	php_info_print("<title>phpinfo()</title>");
	php_info_print("<meta name=\"ROBOTS\" content=\"NOINDEX,NOFOLLOW,NOARCHIVE\" />");
	php_info_print("</head>\n");
	php_info_print("<body><div class=\"center\">\n");