void ldb_req_set_location(struct ldb_request *req, const char *location)
{
	if (req && req->handle) {
		req->handle->location = location;
	}
}