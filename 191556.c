int ldb_rename(struct ldb_context *ldb,
		struct ldb_dn *olddn, struct ldb_dn *newdn)
{
	struct ldb_request *req;
	int ret;

	ret = ldb_build_rename_req(&req, ldb, ldb,
					olddn,
					newdn,
					NULL,
					NULL,
					ldb_op_default_callback,
					NULL);
	ldb_req_set_location(req, "ldb_rename");

	if (ret != LDB_SUCCESS) return ret;

	/* do request and autostart a transaction */
	ret = ldb_autotransaction_request(ldb, req);

	talloc_free(req);
	return ret;
}