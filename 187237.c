static void encode_lock(struct xdr_stream *xdr, const struct nfs_lock_args *args, struct compound_hdr *hdr)
{
	__be32 *p;

	encode_op_hdr(xdr, OP_LOCK, decode_lock_maxsz, hdr);
	p = reserve_space(xdr, 28);
	*p++ = cpu_to_be32(nfs4_lock_type(args->fl, args->block));
	*p++ = cpu_to_be32(args->reclaim);
	p = xdr_encode_hyper(p, args->fl->fl_start);
	p = xdr_encode_hyper(p, nfs4_lock_length(args->fl));
	*p = cpu_to_be32(args->new_lock_owner);
	if (args->new_lock_owner){
		encode_nfs4_seqid(xdr, args->open_seqid);
		encode_nfs4_stateid(xdr, args->open_stateid);
		encode_nfs4_seqid(xdr, args->lock_seqid);
		encode_lockowner(xdr, &args->lock_owner);
	}
	else {
		encode_nfs4_stateid(xdr, args->lock_stateid);
		encode_nfs4_seqid(xdr, args->lock_seqid);
	}
}