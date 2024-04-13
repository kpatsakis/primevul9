int ldb_search(struct ldb_context *ldb, TALLOC_CTX *mem_ctx,
		struct ldb_result **result, struct ldb_dn *base,
		enum ldb_scope scope, const char * const *attrs,
		const char *exp_fmt, ...)
{
	struct ldb_request *req;
	struct ldb_result *res;
	char *expression;
	va_list ap;
	int ret;

	expression = NULL;
	*result = NULL;
	req = NULL;

	res = talloc_zero(mem_ctx, struct ldb_result);
	if (!res) {
		return LDB_ERR_OPERATIONS_ERROR;
	}

	if (exp_fmt) {
		va_start(ap, exp_fmt);
		expression = talloc_vasprintf(mem_ctx, exp_fmt, ap);
		va_end(ap);

		if (!expression) {
			talloc_free(res);
			return LDB_ERR_OPERATIONS_ERROR;
		}
	}

	ret = ldb_build_search_req(&req, ldb, mem_ctx,
					base?base:ldb_get_default_basedn(ldb),
	       				scope,
					expression,
					attrs,
					NULL,
					res,
					ldb_search_default_callback,
					NULL);
	ldb_req_set_location(req, "ldb_search");

	if (ret != LDB_SUCCESS) goto done;

	ret = ldb_request(ldb, req);

	if (ret == LDB_SUCCESS) {
		ret = ldb_wait(req->handle, LDB_WAIT_ALL);
	}

done:
	if (ret != LDB_SUCCESS) {
		talloc_free(res);
		res = NULL;
	}

	talloc_free(expression);
	talloc_free(req);

	*result = res;
	return ret;
}