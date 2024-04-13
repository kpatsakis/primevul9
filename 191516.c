static int lock_search(struct ldb_module *lock_module, struct ldb_request *req)
{
	/* Used in FIRST_OP_NOERR to find where to send the lock request */
	struct ldb_module *next_module = NULL;
	struct ldb_request *down_req = NULL;
	struct ldb_db_lock_context *lock_context;
	struct ldb_context *ldb = ldb_module_get_ctx(lock_module);
	int ret;

	lock_context = talloc(req, struct ldb_db_lock_context);
	if (lock_context == NULL) {
		return ldb_oom(ldb);
	}

	lock_context->ldb = ldb;
	lock_context->req = req;

	ret = ldb_build_search_req_ex(&down_req, ldb, req,
				      req->op.search.base,
				      req->op.search.scope,
				      req->op.search.tree,
				      req->op.search.attrs,
				      req->controls,
				      lock_context,
				      ldb_lock_backend_callback,
				      req);
	LDB_REQ_SET_LOCATION(down_req);
	if (ret != LDB_SUCCESS) {
		return ret;
	}

	/* call DB lock */
	FIRST_OP_NOERR(ldb, read_lock);
	if (next_module != NULL) {
		ret = next_module->ops->read_lock(next_module);
	} else {
		ret = LDB_ERR_UNSUPPORTED_CRITICAL_EXTENSION;
	}

	if (ret == LDB_ERR_UNSUPPORTED_CRITICAL_EXTENSION) {
		/* We might be talking LDAP */
		ldb_reset_err_string(ldb);
		TALLOC_FREE(lock_context);

		return ldb_next_request(lock_module, req);
	} else if ((ret != LDB_SUCCESS) && (ldb->err_string == NULL)) {
		/* if no error string was setup by the backend */
		ldb_asprintf_errstring(ldb, "Failed to get DB lock: %s (%d)",
				       ldb_strerror(ret), ret);
	} else {
		talloc_set_destructor(lock_context, ldb_db_lock_destructor);
	}

	if (ret != LDB_SUCCESS) {
		return ret;
	}

	return ldb_next_request(lock_module, down_req);
}