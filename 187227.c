static int decode_locku(struct xdr_stream *xdr, struct nfs_locku_res *res)
{
	int status;

	status = decode_op_hdr(xdr, OP_LOCKU);
	if (status != -EIO)
		nfs_increment_lock_seqid(status, res->seqid);
	if (status == 0)
		status = decode_stateid(xdr, &res->stateid);
	return status;
}