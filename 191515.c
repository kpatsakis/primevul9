struct tevent_context *ldb_handle_get_event_context(struct ldb_handle *handle)
{
	if (handle->event_context != NULL) {
		return handle->event_context;
	}
	return ldb_get_event_context(handle->ldb);
}