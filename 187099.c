static int decode_commit(struct xdr_stream *xdr, struct nfs_commitres *res)
{
	int status;

	status = decode_op_hdr(xdr, OP_COMMIT);
	if (!status)
		status = decode_write_verifier(xdr, &res->verf->verifier);
	return status;
}