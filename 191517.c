void ldb_req_mark_untrusted(struct ldb_request *req)
{
	req->handle->flags |= LDB_HANDLE_FLAG_UNTRUSTED;
}