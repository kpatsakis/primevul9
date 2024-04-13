int ldb_modify(struct ldb_context *ldb,
	       const struct ldb_message *message)
{
	struct ldb_request *req;
	int ret;

	ret = ldb_msg_sanity_check(ldb, message);
	if (ret != LDB_SUCCESS) {
		return ret;
	}

	ret = ldb_build_mod_req(&req, ldb, ldb,
					message,
					NULL,
					NULL,
					ldb_op_default_callback,
					NULL);
	ldb_req_set_location(req, "ldb_modify");

	if (ret != LDB_SUCCESS) return ret;

	/* do request and autostart a transaction */
	ret = ldb_autotransaction_request(ldb, req);

	talloc_free(req);
	return ret;
}