static int lsql_del_trans(struct ldb_module *module)
{
	struct lsqlite3_private *lsqlite3;

	lsqlite3 = talloc_get_type(ldb_module_get_private(module),
				   struct lsqlite3_private);

	if (lsqlite3->trans_count > 0) {
		lsqlite3->trans_count--;
	} else return -1;

	if (lsqlite3->trans_count == 0) {
		return lsqlite3_safe_rollback(lsqlite3->sqlite);
	}

	return -1;
}