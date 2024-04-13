int ldb_error_at(struct ldb_context *ldb, int ecode,
		 const char *reason, const char *file, int line)
{
	if (reason == NULL) {
		reason = ldb_strerror(ecode);
	}
	ldb_asprintf_errstring(ldb, "%s at %s:%d", reason, file, line);
	return ecode;
}