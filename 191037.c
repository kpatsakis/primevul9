bool req_is_in_chain(struct smb_request *req)
{
	if (req->vwv != (uint16_t *)(req->inbuf+smb_vwv)) {
		/*
		 * We're right now handling a subsequent request, so we must
		 * be in a chain
		 */
		return true;
	}

	if (!is_andx_req(req->cmd)) {
		return false;
	}

	if (req->wct < 2) {
		/*
		 * Okay, an illegal request, but definitely not chained :-)
		 */
		return false;
	}

	return (CVAL(req->vwv+0, 0) != 0xFF);
}