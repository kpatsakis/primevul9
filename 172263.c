static void lsql_timeout(struct tevent_context *ev,
			 struct tevent_timer *te,
			 struct timeval t,
			 void *private_data)
{
	struct lsql_context *ctx;
	ctx = talloc_get_type(private_data, struct lsql_context);

	lsql_request_done(ctx, LDB_ERR_TIME_LIMIT_EXCEEDED);
}