int ldb_build_search_req_ex(struct ldb_request **ret_req,
			struct ldb_context *ldb,
			TALLOC_CTX *mem_ctx,
			struct ldb_dn *base,
	       		enum ldb_scope scope,
			struct ldb_parse_tree *tree,
			const char * const *attrs,
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
		ldb_oom(ldb);
		return LDB_ERR_OPERATIONS_ERROR;
	}

	req->operation = LDB_SEARCH;
	if (base == NULL) {
		req->op.search.base = ldb_dn_new(req, ldb, NULL);
	} else {
		req->op.search.base = base;
	}
	req->op.search.scope = scope;

	req->op.search.tree = tree;
	if (req->op.search.tree == NULL) {
		ldb_set_errstring(ldb, "'tree' can't be NULL");
		talloc_free(req);
		return LDB_ERR_OPERATIONS_ERROR;
	}

	req->op.search.attrs = attrs;
	*ret_req = req;
	return LDB_SUCCESS;
}