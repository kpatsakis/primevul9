int ldb_modify_default_callback(struct ldb_request *req, struct ldb_reply *ares)
{
	struct ldb_result *res;
	unsigned int n;
	int ret;

	res = talloc_get_type(req->context, struct ldb_result);

	if (!ares) {
		return ldb_request_done(req, LDB_ERR_OPERATIONS_ERROR);
	}

	if (ares->error != LDB_SUCCESS) {
		ret = ares->error;
		talloc_free(ares);
		return ldb_request_done(req, ret);
	}

	switch (ares->type) {
	case LDB_REPLY_REFERRAL:
		if (res->refs) {
			for (n = 0; res->refs[n]; n++) /*noop*/ ;
		} else {
			n = 0;
		}

		res->refs = talloc_realloc(res, res->refs, char *, n + 2);
		if (! res->refs) {
			return ldb_request_done(req, LDB_ERR_OPERATIONS_ERROR);
		}

		res->refs[n] = talloc_move(res->refs, &ares->referral);
		res->refs[n + 1] = NULL;
		break;

	case LDB_REPLY_DONE:
		talloc_free(ares);
		return ldb_request_done(req, LDB_SUCCESS);
	default:
		talloc_free(ares);
		ldb_asprintf_errstring(req->handle->ldb, "Invalid LDB reply type %d", ares->type);
		return ldb_request_done(req, LDB_ERR_OPERATIONS_ERROR);
	}

	talloc_free(ares);
	return ldb_request_done(req, LDB_SUCCESS);
}