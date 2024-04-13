void reply_outbuf(struct smb_request *req, uint8 num_words, uint32 num_bytes)
{
	char *outbuf;
	if (!create_outbuf(req, req, (char *)req->inbuf, &outbuf, num_words,
			   num_bytes)) {
		smb_panic("could not allocate output buffer\n");
	}
	req->outbuf = (uint8_t *)outbuf;
}