static void nfs4_xdr_enc_fs_locations(struct rpc_rqst *req,
				      struct xdr_stream *xdr,
				      struct nfs4_fs_locations_arg *args)
{
	struct compound_hdr hdr = {
		.minorversion = nfs4_xdr_minorversion(&args->seq_args),
	};
	uint32_t replen;

	encode_compound_hdr(xdr, req, &hdr);
	encode_sequence(xdr, &args->seq_args, &hdr);
	encode_putfh(xdr, args->dir_fh, &hdr);
	encode_lookup(xdr, args->name, &hdr);
	replen = hdr.replen;	/* get the attribute into args->page */
	encode_fs_locations(xdr, args->bitmask, &hdr);

	xdr_inline_pages(&req->rq_rcv_buf, replen << 2, &args->page,
			0, PAGE_SIZE);
	encode_nops(&hdr);
}