int ldb_build_extended_req(struct ldb_request **ret_req,
			   struct ldb_context *ldb,
			   TALLOC_CTX *mem_ctx,
			   const char *oid,
			   void *data,
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

	req->operation = LDB_EXTENDED;
	req->op.extended.oid = oid;
	req->op.extended.data = data;
	*ret_req = req;
	return LDB_SUCCESS;
}