static struct ldb_request *ldb_build_req_common(TALLOC_CTX *mem_ctx,
				struct ldb_context *ldb,
				struct ldb_control **controls,
				void *context,
				ldb_request_callback_t callback,
				struct ldb_request *parent)
{
	struct ldb_request *req = NULL;

	req = talloc_zero(mem_ctx, struct ldb_request);
	if (req == NULL) {
		return NULL;
	}
	req->controls = controls;
	req->context = context;
	req->callback = callback;

	ldb_set_timeout_from_prev_req(ldb, parent, req);

	if (parent != NULL) {
		req->handle = ldb_handle_new_child(req, parent);
		if (req->handle == NULL) {
			TALLOC_FREE(req);
			return NULL;
		}
	} else {
		req->handle = ldb_handle_new(req, ldb);
		if (req->handle == NULL) {
			TALLOC_FREE(req);
			return NULL;
		}
	}

	return req;
}