daemon_logger(cms_context *cms, int priority, char *fmt, ...)
{
	context *ctx = (context *)cms->log_priv;
	va_list ap;
	int rc = 0;

	if (ctx->errstr)
		xfree(ctx->errstr);

	va_start(ap, fmt);
	if (priority & LOG_ERR) {
		va_list aq;

		va_copy(aq, ap);
		rc = vasprintf(&ctx->errstr, fmt, aq);
		va_end(aq);
	}

	vsyslog(ctx->priority | priority, fmt, ap);
	va_end(ap);
	return rc;
}