int ldb_request_get_status(struct ldb_request *req)
{
	return req->handle->status;
}