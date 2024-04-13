int ldb_build_rename_req(struct ldb_request **ret_req,
			struct ldb_context *ldb,
			TALLOC_CTX *mem_ctx,
			struct ldb_dn *olddn,
			struct ldb_dn *newdn,
			struct ldb_control **controls,
			void *context,
			ldb_request_callback_t callback,
			struct ldb_request *parent)
{
	struct ldb_request *req;

	*ret_req = NULL;

	req = ldb_build_req_common(mem_ctx, ldb, controls,
				   context, callback, parent);
	if (req == NULL) {
		ldb_set_errstring(ldb, "Out of Memory");
		return LDB_ERR_OPERATIONS_ERROR;
	}

	req->operation = LDB_RENAME;
	req->op.rename.olddn = olddn;
	req->op.rename.newdn = newdn;
	*ret_req = req;
	return LDB_SUCCESS;
}