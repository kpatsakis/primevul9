int ldb_op_default_callback(struct ldb_request *req, struct ldb_reply *ares)
{
	int ret;

	if (!ares) {
		return ldb_request_done(req, LDB_ERR_OPERATIONS_ERROR);
	}

	if (ares->error != LDB_SUCCESS) {
		ret = ares->error;
		talloc_free(ares);
		return ldb_request_done(req, ret);
	}

	if (ares->type != LDB_REPLY_DONE) {
		ldb_asprintf_errstring(req->handle->ldb, "Invalid LDB reply type %d", ares->type);
		TALLOC_FREE(ares);
		return ldb_request_done(req, LDB_ERR_OPERATIONS_ERROR);
	}

	talloc_free(ares);
	return ldb_request_done(req, LDB_SUCCESS);
}