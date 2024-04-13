void ldb_asprintf_errstring(struct ldb_context *ldb, const char *format, ...)
{
	va_list ap;
	char *old_err_string = NULL;
	if (ldb->err_string) {
		old_err_string = ldb->err_string;
	}

	va_start(ap, format);
	ldb->err_string = talloc_vasprintf(ldb, format, ap);
	va_end(ap);

	TALLOC_FREE(old_err_string);
	
	if (ldb->flags & LDB_FLG_ENABLE_TRACING) {
		ldb_debug(ldb, LDB_DEBUG_TRACE, "ldb_asprintf/set_errstring: %s",
			  ldb->err_string);
	}
}