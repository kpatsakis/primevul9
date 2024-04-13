int ldb_extended_default_callback(struct ldb_request *req,
				  struct ldb_reply *ares)
{
	struct ldb_result *res;

	res = talloc_get_type(req->context, struct ldb_result);

	if (!ares) {
		return ldb_request_done(req, LDB_ERR_OPERATIONS_ERROR);
	}
	if (ares->error != LDB_SUCCESS) {
		return ldb_request_done(req, ares->error);
	}

	if (ares->type == LDB_REPLY_DONE) {

		/* TODO: we should really support controls on entries and referrals too! */
		res->extended = talloc_move(res, &ares->response);
		res->controls = talloc_move(res, &ares->controls);

		talloc_free(ares);
		return ldb_request_done(req, LDB_SUCCESS);
	}

	talloc_free(ares);
	ldb_asprintf_errstring(req->handle->ldb, "Invalid LDB reply type %d", ares->type);
	return ldb_request_done(req, LDB_ERR_OPERATIONS_ERROR);
}