static void encode_locku(struct xdr_stream *xdr, const struct nfs_locku_args *args, struct compound_hdr *hdr)
{
	__be32 *p;

	encode_op_hdr(xdr, OP_LOCKU, decode_locku_maxsz, hdr);
	encode_uint32(xdr, nfs4_lock_type(args->fl, 0));
	encode_nfs4_seqid(xdr, args->seqid);
	encode_nfs4_stateid(xdr, args->stateid);
	p = reserve_space(xdr, 16);
	p = xdr_encode_hyper(p, args->fl->fl_start);
	xdr_encode_hyper(p, nfs4_lock_length(args->fl));
}