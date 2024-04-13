static struct ldb_handle *ldb_handle_new_child(TALLOC_CTX *mem_ctx,
					       struct ldb_request *parent_req)
{
	struct ldb_handle *h;

	h = talloc_zero(mem_ctx, struct ldb_handle);
	if (h == NULL) {
		ldb_set_errstring(parent_req->handle->ldb,
				  "Out of Memory");
		return NULL;
	}

	h->status = LDB_SUCCESS;
	h->state = LDB_ASYNC_INIT;
	h->ldb = parent_req->handle->ldb;
	h->parent = parent_req;
	h->nesting = parent_req->handle->nesting + 1;
	h->flags = parent_req->handle->flags;
	h->custom_flags = parent_req->handle->custom_flags;
	h->event_context = parent_req->handle->event_context;

	return h;
}