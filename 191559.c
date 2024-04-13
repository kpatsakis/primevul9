int ldb_transaction_cancel(struct ldb_context *ldb)
{
	struct ldb_module *next_module;
	int status;

	ldb->transaction_active--;

	ldb_debug(ldb, LDB_DEBUG_TRACE,
		  "cancel ldb transaction (nesting: %d)",
		  ldb->transaction_active);

	/* really cancel only if all nested transactions are complete */
	if (ldb->transaction_active > 0) {
		return LDB_SUCCESS;
	}

	if (ldb->transaction_active < 0) {
		ldb_debug(ldb, LDB_DEBUG_FATAL,
			  "cancel called but no ldb transactions are active!");
		ldb->transaction_active = 0;
		return LDB_ERR_OPERATIONS_ERROR;
	}

	FIRST_OP(ldb, del_transaction);

	status = next_module->ops->del_transaction(next_module);
	if (status != LDB_SUCCESS) {
		if (ldb->err_string == NULL) {
			/* no error string was setup by the backend */
			ldb_asprintf_errstring(ldb,
				"ldb transaction cancel: %s (%d)",
				ldb_strerror(status),
				status);
		}
		if ((next_module && next_module->ldb->flags & LDB_FLG_ENABLE_TRACING)) {
			ldb_debug(next_module->ldb, LDB_DEBUG_TRACE, "cancel ldb transaction error: %s",
				  ldb_errstring(next_module->ldb));
		}
	}
	return status;
}