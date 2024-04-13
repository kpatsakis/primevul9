void ldb_reset_err_string(struct ldb_context *ldb)
{
	if (ldb->err_string) {
		talloc_free(ldb->err_string);
		ldb->err_string = NULL;
	}
}