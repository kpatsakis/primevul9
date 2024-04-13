int ldb_search_default_callback(struct ldb_request *req,
				struct ldb_reply *ares)
{
	struct ldb_result *res;
	unsigned int n;

	res = talloc_get_type(req->context, struct ldb_result);

	if (!ares) {
		return ldb_request_done(req, LDB_ERR_OPERATIONS_ERROR);
	}
	if (ares->error != LDB_SUCCESS) {
		return ldb_request_done(req, ares->error);
	}

	switch (ares->type) {
	case LDB_REPLY_ENTRY:
		res->msgs = talloc_realloc(res, res->msgs,
					struct ldb_message *, res->count + 2);
		if (! res->msgs) {
			return ldb_request_done(req, LDB_ERR_OPERATIONS_ERROR);
		}

		res->msgs[res->count + 1] = NULL;

		res->msgs[res->count] = talloc_move(res->msgs, &ares->message);
		res->count++;
		break;

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
		/* TODO: we should really support controls on entries
		 * and referrals too! */
		res->controls = talloc_move(res, &ares->controls);

		/* this is the last message, and means the request is done */
		/* we have to signal and eventual ldb_wait() waiting that the
		 * async request operation was completed */
		talloc_free(ares);
		return ldb_request_done(req, LDB_SUCCESS);
	}

	talloc_free(ares);

	return LDB_SUCCESS;
}