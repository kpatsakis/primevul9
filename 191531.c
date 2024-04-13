struct ldb_handle *ldb_handle_new(TALLOC_CTX *mem_ctx, struct ldb_context *ldb)
{
	struct ldb_handle *h;

	h = talloc_zero(mem_ctx, struct ldb_handle);
	if (h == NULL) {
		ldb_set_errstring(ldb, "Out of Memory");
		return NULL;
	}

	h->status = LDB_SUCCESS;
	h->state = LDB_ASYNC_INIT;
	h->ldb = ldb;
	h->flags = 0;
	h->location = NULL;
	h->parent = NULL;

	if (h->ldb->require_private_event_context == true) {
		h->event_context = tevent_context_init(h);
		if (h->event_context == NULL) {
			ldb_set_errstring(ldb,
					  "Out of Memory allocating "
					  "event context for new handle");
			return NULL;
		}
		tevent_set_debug(h->event_context, ldb_tevent_debug, ldb);
		tevent_loop_allow_nesting(h->event_context);
	}

	return h;
}