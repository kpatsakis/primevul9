int ldb_extended(struct ldb_context *ldb,
		 const char *oid,
		 void *data,
		 struct ldb_result **_res)
{
	struct ldb_request *req;
	int ret;
	struct ldb_result *res;

	*_res = NULL;
	req = NULL;

	res = talloc_zero(ldb, struct ldb_result);
	if (!res) {
		return LDB_ERR_OPERATIONS_ERROR;
	}

	ret = ldb_build_extended_req(&req, ldb, ldb,
				     oid, data, NULL,
				     res, ldb_extended_default_callback,
				     NULL);
	ldb_req_set_location(req, "ldb_extended");

	if (ret != LDB_SUCCESS) goto done;

	ldb_set_timeout(ldb, req, 0); /* use default timeout */

	ret = ldb_request(ldb, req);

	if (ret == LDB_SUCCESS) {
		ret = ldb_wait(req->handle, LDB_WAIT_ALL);
	}

done:
	if (ret != LDB_SUCCESS) {
		talloc_free(res);
		res = NULL;
	}

	talloc_free(req);

	*_res = res;
	return ret;
}