PHP_LIBXML_API void php_libxml_ctx_error(void *ctx, const char *msg, ...)
{
	va_list args;
	va_start(args, msg);
	php_libxml_internal_error_handler(PHP_LIBXML_CTX_ERROR, ctx, &msg, args);
	va_end(args);
}