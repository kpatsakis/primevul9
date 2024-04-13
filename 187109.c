static void encode_setattr(struct xdr_stream *xdr, const struct nfs_setattrargs *arg, const struct nfs_server *server, struct compound_hdr *hdr)
{
	encode_op_hdr(xdr, OP_SETATTR, decode_setattr_maxsz, hdr);
	encode_nfs4_stateid(xdr, &arg->stateid);
	encode_attrs(xdr, arg->iap, server);
}