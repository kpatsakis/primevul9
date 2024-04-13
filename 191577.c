void ldb_set_errstring(struct ldb_context *ldb, const char *err_string)
{
	ldb_asprintf_errstring(ldb, "%s", err_string);
}