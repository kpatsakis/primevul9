int ldb_build_search_req(struct ldb_request **ret_req,
			struct ldb_context *ldb,
			TALLOC_CTX *mem_ctx,
			struct ldb_dn *base,
			enum ldb_scope scope,
			const char *expression,
			const char * const *attrs,
			struct ldb_control **controls,
			void *context,
			ldb_request_callback_t callback,
			struct ldb_request *parent)
{
	struct ldb_parse_tree *tree;
	int ret;

	tree = ldb_parse_tree(mem_ctx, expression);
	if (tree == NULL) {
		ldb_set_errstring(ldb, "Unable to parse search expression");
		return LDB_ERR_OPERATIONS_ERROR;
	}

	ret = ldb_build_search_req_ex(ret_req, ldb, mem_ctx, base,
				      scope, tree, attrs, controls,
				      context, callback, parent);
	if (ret == LDB_SUCCESS) {
		talloc_steal(*ret_req, tree);
	}
	return ret;
}