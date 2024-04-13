int ldb_transaction_cancel_noerr(struct ldb_context *ldb)
{
	if (ldb->transaction_active > 0) {
		return ldb_transaction_cancel(ldb);
	}
	return LDB_SUCCESS;
}