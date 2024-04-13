void ldb_handle_use_global_event_context(struct ldb_handle *handle)
{
	TALLOC_FREE(handle->event_context);
}