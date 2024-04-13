int ldb_transaction_commit(struct ldb_context *ldb)
{
	struct ldb_module *next_module;
	int status;

	status = ldb_transaction_prepare_commit(ldb);
	if (status != LDB_SUCCESS) {
		return status;
	}

	ldb->transaction_active--;

	ldb_debug(ldb, LDB_DEBUG_TRACE,
		  "commit ldb transaction (nesting: %d)",
		  ldb->transaction_active);

	/* commit only when all nested transactions are complete */
	if (ldb->transaction_active > 0) {
		return LDB_SUCCESS;
	}

	if (ldb->transaction_active < 0) {
		ldb_debug(ldb, LDB_DEBUG_FATAL,
			  "commit called but no ldb transactions are active!");
		ldb->transaction_active = 0;
		return LDB_ERR_OPERATIONS_ERROR;
	}

	ldb_reset_err_string(ldb);

	FIRST_OP(ldb, end_transaction);
	status = next_module->ops->end_transaction(next_module);
	if (status != LDB_SUCCESS) {
		if (ldb->err_string == NULL) {
			/* no error string was setup by the backend */
			ldb_asprintf_errstring(ldb,
				"ldb transaction commit: %s (%d)",
				ldb_strerror(status),
				status);
		}
		if ((next_module && next_module->ldb->flags & LDB_FLG_ENABLE_TRACING)) {
			ldb_debug(next_module->ldb, LDB_DEBUG_TRACE, "commit ldb transaction error: %s",
				  ldb_errstring(next_module->ldb));
		}
	}
	return status;
}