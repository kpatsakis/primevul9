int ldb_delete(struct ldb_context *ldb, struct ldb_dn *dn)
{
	struct ldb_request *req;
	int ret;

	ret = ldb_build_del_req(&req, ldb, ldb,
					dn,
					NULL,
					NULL,
					ldb_op_default_callback,
					NULL);
	ldb_req_set_location(req, "ldb_delete");

	if (ret != LDB_SUCCESS) return ret;

	/* do request and autostart a transaction */
	ret = ldb_autotransaction_request(ldb, req);

	talloc_free(req);
	return ret;
}