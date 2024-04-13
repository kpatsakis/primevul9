void ldb_set_event_context(struct ldb_context *ldb, struct tevent_context *ev)
{
	ldb->ev_ctx = ev;
}