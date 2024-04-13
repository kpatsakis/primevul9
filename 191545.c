const char *ldb_errstring(struct ldb_context *ldb)
{
	if (ldb->err_string) {
		return ldb->err_string;
	}

	return NULL;
}