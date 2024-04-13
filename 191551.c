int ldb_set_timeout_from_prev_req(struct ldb_context *ldb,
				  struct ldb_request *oldreq,
				  struct ldb_request *newreq)
{
	if (newreq == NULL) return LDB_ERR_OPERATIONS_ERROR;

	if (oldreq == NULL) {
		return ldb_set_timeout(ldb, newreq, 0);
	}

	newreq->starttime = oldreq->starttime;
	newreq->timeout = oldreq->timeout;

	return LDB_SUCCESS;
}