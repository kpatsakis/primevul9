static int lsql_rename(struct lsql_context *ctx)
{
	struct ldb_module *module = ctx->module;
	struct ldb_request *req = ctx->req;
	struct lsqlite3_private *lsqlite3;
	struct ldb_context *ldb;
	char *new_dn, *new_cdn, *old_cdn;
	char *errmsg;
	char *query;
	int ret;

	ldb = ldb_module_get_ctx(module);
	lsqlite3 = talloc_get_type(ldb_module_get_private(module),
				   struct lsqlite3_private);

	/* create linearized and normalized dns */
	old_cdn = ldb_dn_alloc_casefold(ctx, req->op.rename.olddn);
	new_cdn = ldb_dn_alloc_casefold(ctx, req->op.rename.newdn);
	new_dn = ldb_dn_alloc_linearized(ctx, req->op.rename.newdn);
	if (old_cdn == NULL || new_cdn == NULL || new_dn == NULL) {
		return LDB_ERR_OPERATIONS_ERROR;
	}

	/* build the SQL query */
	query = lsqlite3_tprintf(ctx,
				 "UPDATE ldb_entry SET dn = '%q', norm_dn = '%q' "
				 "WHERE norm_dn = '%q';",
				 new_dn, new_cdn, old_cdn);
	if (query == NULL) {
		return LDB_ERR_OPERATIONS_ERROR;
	}

	/* execute */
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