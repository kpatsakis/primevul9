static inline void encode_openhdr(struct xdr_stream *xdr, const struct nfs_openargs *arg)
{
	__be32 *p;
 /*
 * opcode 4, seqid 4, share_access 4, share_deny 4, clientid 8, ownerlen 4,
 * owner 4 = 32
 */
	encode_nfs4_seqid(xdr, arg->seqid);
	encode_share_access(xdr, arg->fmode);
	p = reserve_space(xdr, 36);
	p = xdr_encode_hyper(p, arg->clientid);
	*p++ = cpu_to_be32(24);
	p = xdr_encode_opaque_fixed(p, "open id:", 8);
	*p++ = cpu_to_be32(arg->server->s_dev);
	*p++ = cpu_to_be32(arg->id.uniquifier);
	xdr_encode_hyper(p, arg->id.create_time);
}