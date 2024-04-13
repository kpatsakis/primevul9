int ldb_wait(struct ldb_handle *handle, enum ldb_wait_type type)
{
	struct tevent_context *ev;
	int ret;

	if (handle == NULL) {
		return LDB_ERR_UNAVAILABLE;
	}

	if (handle->state == LDB_ASYNC_DONE) {
		if ((handle->status != LDB_SUCCESS) &&
		    (handle->ldb->err_string == NULL)) {
			/* if no error string was setup by the backend */
			ldb_asprintf_errstring(handle->ldb,
					       "ldb_wait from %s with LDB_ASYNC_DONE: %s (%d)",
					       handle->location,
					       ldb_strerror(handle->status),
					       handle->status);
		}
		return handle->status;
	}

	ev = ldb_handle_get_event_context(handle);
	if (NULL == ev) {
		return ldb_oom(handle->ldb);
	}

	switch (type) {
	case LDB_WAIT_NONE:
		ret = tevent_loop_once(ev);
		if (ret != 0) {
			return ldb_operr(handle->ldb);
		}
		if (handle->status == LDB_SUCCESS) {
			return LDB_SUCCESS;
		}
		if (handle->ldb->err_string != NULL) {
			return handle->status;
		}
		/*
		 * if no error string was setup by the backend
		 */
		ldb_asprintf_errstring(handle->ldb,
				       "ldb_wait from %s with LDB_WAIT_NONE: %s (%d)",
				       handle->location,
				       ldb_strerror(handle->status),
				       handle->status);
		return handle->status;

	case LDB_WAIT_ALL:
		while (handle->state != LDB_ASYNC_DONE) {
			ret = tevent_loop_once(ev);
			if (ret != 0) {
				return ldb_operr(handle->ldb);
			}
			if (handle->status != LDB_SUCCESS) {
				if (handle->ldb->err_string != NULL) {
					return handle->status;
				}
				/*
				 * if no error string was setup by the
				 * backend
				 */
				ldb_asprintf_errstring(handle->ldb,
						       "ldb_wait from %s with "
						       "LDB_WAIT_ALL: %s (%d)",
						       handle->location,
						       ldb_strerror(handle->status),
						       handle->status);
				return handle->status;
			}
		}
		if (handle->status == LDB_SUCCESS) {
			return LDB_SUCCESS;
		}
		if (handle->ldb->err_string != NULL) {
			return handle->status;
		}
		/*
		 * if no error string was setup by the backend
		 */
		ldb_asprintf_errstring(handle->ldb,
				       "ldb_wait from %s with LDB_WAIT_ALL,"
				       " LDB_ASYNC_DONE: %s (%d)",
				       handle->location,
				       ldb_strerror(handle->status),
				       handle->status);
		return handle->status;
	}

	return LDB_SUCCESS;
}