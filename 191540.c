static int ldb_autotransaction_request(struct ldb_context *ldb,
				       struct ldb_request *req)
{
	int ret;

	ret = ldb_transaction_start(ldb);
	if (ret != LDB_SUCCESS) {
		return ret;
	}

	ret = ldb_request(ldb, req);
	if (ret == LDB_SUCCESS) {
		ret = ldb_wait(req->handle, LDB_WAIT_ALL);
	}

	if (ret == LDB_SUCCESS) {
		return ldb_transaction_commit(ldb);
	}
	ldb_transaction_cancel(ldb);

	return ret;
}