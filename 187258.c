static int nfs4_xdr_dec_secinfo_no_name(struct rpc_rqst *rqstp,
					struct xdr_stream *xdr,
					struct nfs4_secinfo_res *res)
{
	struct compound_hdr hdr;
	int status;

	status = decode_compound_hdr(xdr, &hdr);
	if (status)
		goto out;
	status = decode_sequence(xdr, &res->seq_res, rqstp);
	if (status)
		goto out;
	status = decode_putrootfh(xdr);
	if (status)
		goto out;
	status = decode_secinfo_no_name(xdr, res);
out:
	return status;
}