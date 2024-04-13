void ldb_req_set_custom_flags(struct ldb_request *req, uint32_t flags)
{
	if (req != NULL && req->handle != NULL) {
		req->handle->custom_flags = flags;
	}
}