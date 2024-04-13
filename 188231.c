
static int php_info_printf(const char *fmt, ...) /* {{{ */
{
	char *buf;
	int len, written;
	va_list argv;
	TSRMLS_FETCH();

	va_start(argv, fmt);
	len = vspprintf(&buf, 0, fmt, argv);
	va_end(argv);

	written = php_output_write(buf, len TSRMLS_CC);
	efree(buf);
	return written;