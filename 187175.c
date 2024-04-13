encode_getdevicelist(struct xdr_stream *xdr,
		     const struct nfs4_getdevicelist_args *args,
		     struct compound_hdr *hdr)
{
	__be32 *p;
	nfs4_verifier dummy = {
		.data = "dummmmmy",
	};

	encode_op_hdr(xdr, OP_GETDEVICELIST, decode_getdevicelist_maxsz, hdr);
	p = reserve_space(xdr, 16);
	*p++ = cpu_to_be32(args->layoutclass);
	*p++ = cpu_to_be32(NFS4_PNFS_GETDEVLIST_MAXNUM);
	xdr_encode_hyper(p, 0ULL);                          /* cookie */
	encode_nfs4_verifier(xdr, &dummy);
}