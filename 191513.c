struct tevent_context * ldb_get_event_context(struct ldb_context *ldb)
{
	return ldb->ev_ctx;
}