void ldb_set_require_private_event_context(struct ldb_context *ldb)
{
	ldb->require_private_event_context = true;
}