static void nfs4_xdr_enc_readlink(struct rpc_rqst *req, struct xdr_stream *xdr,
				  const struct nfs4_readlink *args)
{
	struct compound_hdr hdr = {
		.minorversion = nfs4_xdr_minorversion(&args->seq_args),
	};

	encode_compound_hdr(xdr, req, &hdr);
	encode_sequence(xdr, &args->seq_args, &hdr);
	encode_putfh(xdr, args->fh, &hdr);
	encode_readlink(xdr, args, req, &hdr);

	xdr_inline_pages(&req->rq_rcv_buf, hdr.replen << 2, args->pages,
			args->pgbase, args->pglen);
	encode_nops(&hdr);
}