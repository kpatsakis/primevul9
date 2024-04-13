static int lsqlite3_connect(struct ldb_context *ldb,
			    const char *url,
			    unsigned int flags,
			    const char *options[],
			    struct ldb_module **_module)
{
	struct ldb_module *module;
	struct lsqlite3_private *lsqlite3;
	unsigned int i;
	int ret;

	module = ldb_module_new(ldb, ldb, "ldb_sqlite3 backend", &lsqlite3_ops);
	if (!module) return LDB_ERR_OPERATIONS_ERROR;

	lsqlite3 = talloc(module, struct lsqlite3_private);
	if (!lsqlite3) {
		goto failed;
	}

	lsqlite3->sqlite = NULL;
	lsqlite3->options = NULL;
	lsqlite3->trans_count = 0;

	ret = initialize(lsqlite3, ldb, url, flags);
	if (ret != SQLITE_OK) {
		goto failed;
	}

	talloc_set_destructor(lsqlite3, destructor);

	ldb_module_set_private(module, lsqlite3);

	if (options) {
		/*
                 * take a copy of the options array, so we don't have to rely
                 * on the caller keeping it around (it might be dynamic)
                 */
		for (i=0;options[i];i++) ;

		lsqlite3->options = talloc_array(lsqlite3, char *, i+1);
		if (!lsqlite3->options) {
			goto failed;
		}

		for (i=0;options[i];i++) {

			lsqlite3->options[i+1] = NULL;
			lsqlite3->options[i] =
                                talloc_strdup(lsqlite3->options, options[i]);
			if (!lsqlite3->options[i]) {
				goto failed;
			}
		}
	}

	*_module = module;
	return LDB_SUCCESS;

failed:
        if (lsqlite3 && lsqlite3->sqlite != NULL) {
                (void) sqlite3_close(lsqlite3->sqlite);
        }
	talloc_free(lsqlite3);
	return LDB_ERR_OPERATIONS_ERROR;
}