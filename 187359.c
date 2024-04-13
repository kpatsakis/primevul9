static int decode_open_confirm(struct xdr_stream *xdr, struct nfs_open_confirmres *res)
{
	int status;

	status = decode_op_hdr(xdr, OP_OPEN_CONFIRM);
	if (status != -EIO)
		nfs_increment_open_seqid(status, res->seqid);
	if (!status)
		status = decode_stateid(xdr, &res->stateid);
	return status;
}