int ldb_request_done(struct ldb_request *req, int status)
{
	req->handle->state = LDB_ASYNC_DONE;
	req->handle->status = status;
	return status;
}