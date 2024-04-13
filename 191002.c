static void fixup_chain_error_packet(struct smb_request *req)
{
	uint8_t *outbuf = req->outbuf;
	req->outbuf = NULL;
	reply_outbuf(req, 2, 0);
	memcpy(req->outbuf, outbuf, smb_wct);
	TALLOC_FREE(outbuf);
	SCVAL(req->outbuf, smb_vwv0, 0xff);
}