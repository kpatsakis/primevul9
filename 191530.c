int ldb_set_timeout(struct ldb_context *ldb,
		    struct ldb_request *req,
		    int timeout)
{
	if (req == NULL) return LDB_ERR_OPERATIONS_ERROR;

	if (timeout != 0) {
		req->timeout = timeout;
	} else {
		req->timeout = ldb->default_timeout;
	}
	req->starttime = time(NULL);

	return LDB_SUCCESS;
}