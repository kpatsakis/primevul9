static int lsql_handle_request(struct ldb_module *module, struct ldb_request *req)
{
	struct ldb_context *ldb;
	struct tevent_context *ev;
	struct lsql_context *ac;
	struct tevent_timer *te;
	struct timeval tv;

	if (ldb_check_critical_controls(req->controls)) {
		return LDB_ERR_UNSUPPORTED_CRITICAL_EXTENSION;
	}

	if (req->starttime == 0 || req->timeout == 0) {
		ldb_set_errstring(ldb, "Invalid timeout settings");
		return LDB_ERR_TIME_LIMIT_EXCEEDED;
	}

	ldb = ldb_module_get_ctx(module);
	ev = ldb_get_event_context(ldb);

	ac = talloc_zero(req, struct lsql_context);
	if (ac == NULL) {
		ldb_set_errstring(ldb, "Out of Memory");
		return LDB_ERR_OPERATIONS_ERROR;
	}

	ac->module = module;
	ac->req = req;

	tv.tv_sec = 0;
	tv.tv_usec = 0;
	te = tevent_add_timer(ev, ac, tv, lsql_callback, ac);
	if (NULL == te) {
		return LDB_ERR_OPERATIONS_ERROR;
	}

	if (req->timeout > 0) {
		tv.tv_sec = req->starttime + req->timeout;
		tv.tv_usec = 0;
		ac->timeout_event = tevent_add_timer(ev, ac, tv, lsql_timeout, ac);
		if (NULL == ac->timeout_event) {
			return LDB_ERR_OPERATIONS_ERROR;
		}
	}

	return LDB_SUCCESS;
}