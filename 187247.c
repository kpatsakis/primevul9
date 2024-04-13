static void encode_open_downgrade(struct xdr_stream *xdr, const struct nfs_closeargs *arg, struct compound_hdr *hdr)
{
	encode_op_hdr(xdr, OP_OPEN_DOWNGRADE, decode_open_downgrade_maxsz, hdr);
	encode_nfs4_stateid(xdr, arg->stateid);
	encode_nfs4_seqid(xdr, arg->seqid);
	encode_share_access(xdr, arg->fmode);
}