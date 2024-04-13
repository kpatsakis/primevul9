const char *ldb_req_location(struct ldb_request *req)
{
	return req->handle->location;
}