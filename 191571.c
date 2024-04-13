int ldb_transaction_start(struct ldb_context *ldb)
{
	struct ldb_module *next_module;
	int status;

	ldb_debug(ldb, LDB_DEBUG_TRACE,
		  "start ldb transaction (nesting: %d)",
		  ldb->transaction_active);

	/* explicit transaction active, count nested requests */
	if (ldb->transaction_active) {
		ldb->transaction_active++;
		return LDB_SUCCESS;
	}

	/* start a new transaction */
	ldb->transaction_active++;
	ldb->prepare_commit_done = false;

	FIRST_OP(ldb, start_transaction);

	ldb_reset_err_string(ldb);

	status = next_module->ops->start_transaction(next_module);
	if (status != LDB_SUCCESS) {
		if (ldb->err_string == NULL) {
			/* no error string was setup by the backend */
			ldb_asprintf_errstring(ldb,
				"ldb transaction start: %s (%d)",
				ldb_strerror(status),
				status);
		ldb->transaction_active--;
		}
		if ((next_module && next_module->ldb->flags & LDB_FLG_ENABLE_TRACING)) {
			ldb_debug(next_module->ldb, LDB_DEBUG_TRACE, "start ldb transaction error: %s",
				  ldb_errstring(next_module->ldb));
		}
	} else {
		if ((next_module && next_module->ldb->flags & LDB_FLG_ENABLE_TRACING)) {
			ldb_debug(next_module->ldb, LDB_DEBUG_TRACE, "start ldb transaction success");
		}
	}
	return status;
}