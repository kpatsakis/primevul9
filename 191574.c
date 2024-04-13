void ldb_request_set_state(struct ldb_request *req, int state)
{
	req->handle->state = state;
}