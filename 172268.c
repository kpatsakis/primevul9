static int lsql_end_trans(struct ldb_module *module)
{
	int ret;
	char *errmsg;
	struct lsqlite3_private *lsqlite3;

	lsqlite3 = talloc_get_type(ldb_module_get_private(module),
				   struct lsqlite3_private);

	if (lsqlite3->trans_count > 0) {
		lsqlite3->trans_count--;
	} else return -1;

	if (lsqlite3->trans_count == 0) {
		ret = sqlite3_exec(lsqlite3->sqlite, "COMMIT;", NULL, NULL, &errmsg);
		if (ret != SQLITE_OK) {
			if (errmsg) {
				printf("lsqlite3_end_trans: error: %s\n", errmsg);
				free(errmsg);
			}
			return -1;
		}
	}

        return 0;
}