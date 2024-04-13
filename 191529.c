uint32_t ldb_req_get_custom_flags(struct ldb_request *req)
{
	if (req != NULL && req->handle != NULL) {
		return req->handle->custom_flags;
	}

	/*
	 * 0 is not something any better or worse than
	 * anything else as req or the handle is NULL
	 */
	return 0;
}