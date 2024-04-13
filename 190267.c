static void _php_iconv_show_error(php_iconv_err_t err, const char *out_charset, const char *in_charset TSRMLS_DC)
{
	switch (err) {
		case PHP_ICONV_ERR_SUCCESS:
			break;

		case PHP_ICONV_ERR_CONVERTER:
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Cannot open converter");
			break;

		case PHP_ICONV_ERR_WRONG_CHARSET:
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Wrong charset, conversion from `%s' to `%s' is not allowed",
			          in_charset, out_charset);
			break;

		case PHP_ICONV_ERR_ILLEGAL_CHAR:
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Detected an incomplete multibyte character in input string");
			break;

		case PHP_ICONV_ERR_ILLEGAL_SEQ:
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Detected an illegal character in input string");
			break;

		case PHP_ICONV_ERR_TOO_BIG:
			/* should not happen */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Buffer length exceeded");
			break;

		case PHP_ICONV_ERR_MALFORMED:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Malformed string");
			break;

		default:
			/* other error */
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Unknown error (%d)", errno);
			break;
	}
}