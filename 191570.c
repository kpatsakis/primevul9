static int ldb_context_destructor(void *ptr)
{
	struct ldb_context *ldb = talloc_get_type(ptr, struct ldb_context);

	if (ldb->transaction_active) {
		ldb_debug(ldb, LDB_DEBUG_FATAL,
			  "A transaction is still active in ldb context [%p] on %s",
			  ldb, (const char *)ldb_get_opaque(ldb, "ldb_url"));
	}

	return 0;
}