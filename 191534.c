bool ldb_req_is_untrusted(struct ldb_request *req)
{
	return (req->handle->flags & LDB_HANDLE_FLAG_UNTRUSTED) != 0;
}