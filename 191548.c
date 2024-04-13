struct ldb_context *ldb_init(TALLOC_CTX *mem_ctx, struct tevent_context *ev_ctx)
{
	struct ldb_context *ldb;
	int ret;
	const char *modules_path = getenv("LDB_MODULES_PATH");

	if (modules_path == NULL) {
		modules_path = LDB_MODULESDIR;
	}

	ret = ldb_modules_load(modules_path, LDB_VERSION);
	if (ret != LDB_SUCCESS) {
		return NULL;
	}

	ldb = talloc_zero(mem_ctx, struct ldb_context);
	if (ldb == NULL) {
		return NULL;
	}

	/* A new event context so that callers who don't want ldb
	 * operating on their global event context can work without
	 * having to provide their own private one explicitly */
	if (ev_ctx == NULL) {
		ev_ctx = tevent_context_init(ldb);
		if (ev_ctx == NULL) {
			talloc_free(ldb);
			return NULL;
		}
		tevent_set_debug(ev_ctx, ldb_tevent_debug, ldb);
		tevent_loop_allow_nesting(ev_ctx);
	}

	ret = ldb_setup_wellknown_attributes(ldb);
	if (ret != LDB_SUCCESS) {
		talloc_free(ldb);
		return NULL;
	}

	ldb_set_utf8_default(ldb);
	ldb_set_create_perms(ldb, 0666);
	ldb_set_modules_dir(ldb, LDB_MODULESDIR);
	ldb_set_event_context(ldb, ev_ctx);
	ret = ldb_register_extended_match_rules(ldb);
	if (ret != LDB_SUCCESS) {
		talloc_free(ldb);
		return NULL;
	}

	/* TODO: get timeout from options if available there */
	ldb->default_timeout = 300; /* set default to 5 minutes */

	talloc_set_destructor((TALLOC_CTX *)ldb, ldb_context_destructor);

	return ldb;
}