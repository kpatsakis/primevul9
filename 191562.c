int ldb_transaction_prepare_commit(struct ldb_context *ldb)
{
	struct ldb_module *next_module;
	int status;

	if (ldb->prepare_commit_done) {
		return LDB_SUCCESS;
	}

	/* commit only when all nested transactions are complete */
	if (ldb->transaction_active > 1) {
		return LDB_SUCCESS;
	}

	ldb->prepare_commit_done = true;

	if (ldb->transaction_active < 0) {
		ldb_debug(ldb, LDB_DEBUG_FATAL,
			  "prepare commit called but no ldb transactions are active!");
		ldb->transaction_active = 0;
		return LDB_ERR_OPERATIONS_ERROR;
	}

	/* call prepare transaction if available */
	FIRST_OP_NOERR(ldb, prepare_commit);
	if (next_module == NULL) {
		return LDB_SUCCESS;
	}

	ldb_reset_err_string(ldb);

	status = next_module->ops->prepare_commit(next_module);
	if (status != LDB_SUCCESS) {
		ldb->transaction_active--;
		/* if a next_module fails the prepare then we need
		   to call the end transaction for everyone */
		FIRST_OP(ldb, del_transaction);
		next_module->ops->del_transaction(next_module);
		if (ldb->err_string == NULL) {
			/* no error string was setup by the backend */
			ldb_asprintf_errstring(ldb,
					       "ldb transaction prepare commit: %s (%d)",
					       ldb_strerror(status),
					       status);
		}
		if ((next_module && next_module->ldb->flags & LDB_FLG_ENABLE_TRACING)) {
			ldb_debug(next_module->ldb, LDB_DEBUG_TRACE, "prepare commit transaction error: %s",
				  ldb_errstring(next_module->ldb));
		}
	}

	return status;
}