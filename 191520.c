int ldb_connect(struct ldb_context *ldb, const char *url,
		unsigned int flags, const char *options[])
{
	int ret;
	char *url2;
	/* We seem to need to do this here, or else some utilities don't
	 * get ldb backends */

	ldb->flags = flags;

	url2 = talloc_strdup(ldb, url);
	if (!url2) {
		ldb_oom(ldb);
		return LDB_ERR_OPERATIONS_ERROR;
	}
	ret = ldb_set_opaque(ldb, "ldb_url", url2);
	if (ret != LDB_SUCCESS) {
		return ret;
	}

	/*
	 * Take a copy of the options.
	 */
	ldb->options = ldb_options_copy(ldb, options);
	if (ldb->options == NULL && options != NULL) {
		ldb_oom(ldb);
		return LDB_ERR_OPERATIONS_ERROR;
	}

	ret = ldb_module_connect_backend(ldb, url, options, &ldb->modules);
	if (ret != LDB_SUCCESS) {
		return ret;
	}

	ret = ldb_load_modules(ldb, options);
	if (ret != LDB_SUCCESS) {
		ldb_debug(ldb, LDB_DEBUG_FATAL,
			  "Unable to load modules for %s: %s",
			  url, ldb_errstring(ldb));
		return ret;
	}

	/* set the default base dn */
	ldb_set_default_dns(ldb);

	return LDB_SUCCESS;
}