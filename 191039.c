size_t req_wct_ofs(struct smb_request *req)
{
	size_t buf_size;

	if (req->chain_outbuf == NULL) {
		return smb_wct - 4;
	}
	buf_size = talloc_get_size(req->chain_outbuf);
	if ((buf_size % 4) != 0) {
		buf_size += (4 - (buf_size % 4));
	}
	return buf_size - 4;
}