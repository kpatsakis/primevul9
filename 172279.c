static void lsql_callback(struct tevent_context *ev,
			  struct tevent_timer *te,
			  struct timeval t,
			  void *private_data)
{
	struct lsql_context *ctx;
	int ret;

	ctx = talloc_get_type(private_data, struct lsql_context);

	switch (ctx->req->operation) {
	case LDB_SEARCH:
		ret = lsql_search(ctx);
		break;
	case LDB_ADD:
		ret = lsql_add(ctx);
		break;
	case LDB_MODIFY:
		ret = lsql_modify(ctx);
		break;
	case LDB_DELETE:
		ret = lsql_delete(ctx);
		break;
	case LDB_RENAME:
		ret = lsql_rename(ctx);
		break;
/* TODO:
	case LDB_EXTENDED:
		ret = lsql_extended(ctx);
		break;
 */
	default:
		/* no other op supported */
		ret = LDB_ERR_PROTOCOL_ERROR;
	}

	if (!ctx->callback_failed) {
		/* Once we are done, we do not need timeout events */
		talloc_free(ctx->timeout_event);
		lsql_request_done(ctx, ret);
	}
}