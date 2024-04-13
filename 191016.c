void construct_reply_common_req(struct smb_request *req, char *outbuf)
{
	construct_reply_common(req, (char *)req->inbuf, outbuf);
}