static int lsql_delete(struct lsql_context *ctx)
{
	struct ldb_module *module = ctx->module;
	struct ldb_request *req = ctx->req;
	struct lsqlite3_private *lsqlite3;
	struct ldb_context *ldb;
        long long eid;
	char *errmsg;
	char *query;
	int ret;

	ldb = ldb_module_get_ctx(module);
	lsqlite3 = talloc_get_type(ldb_module_get_private(module),
				   struct lsqlite3_private);

	eid = lsqlite3_get_eid(lsqlite3, req->op.del.dn);
	if (eid == -1) {
		return LDB_ERR_OPERATIONS_ERROR;
	}

	query = lsqlite3_tprintf(ctx,
				   /* Delete entry */
				   "DELETE FROM ldb_entry WHERE eid = %lld; "
				   /* Delete attributes */
				   "DELETE FROM ldb_attribute_values WHERE eid = %lld; ",
				eid, eid);
	if (query == NULL) {
		return LDB_ERR_OPERATIONS_ERROR;
	}

	ret = sqlite3_exec(lsqlite3->sqlite, query, NULL, NULL, &errmsg);
	if (ret != SQLITE_OK) {
		if (errmsg) {
			ldb_set_errstring(ldb, errmsg);
			free(errmsg);
		}
		return LDB_ERR_OPERATIONS_ERROR;
	}

	return LDB_SUCCESS;
}