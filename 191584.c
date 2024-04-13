static int ldb_db_lock_destructor(struct ldb_db_lock_context *lock_context)
{
	int ret;
	struct ldb_module *next_module;
	FIRST_OP_NOERR(lock_context->ldb, read_unlock);
	if (next_module != NULL) {
		ret = next_module->ops->read_unlock(next_module);
	} else {
		ret = LDB_SUCCESS;
	}

	if (ret != LDB_SUCCESS) {
		ldb_debug(lock_context->ldb,
			  LDB_DEBUG_FATAL,
			  "Failed to unlock db: %s / %s",
			  ldb_errstring(lock_context->ldb),
			  ldb_strerror(ret));
	}
	return 0;
}