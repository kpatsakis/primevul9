static void encode_close(struct xdr_stream *xdr, const struct nfs_closeargs *arg, struct compound_hdr *hdr)
{
	encode_op_hdr(xdr, OP_CLOSE, decode_close_maxsz, hdr);
	encode_nfs4_seqid(xdr, arg->seqid);
	encode_nfs4_stateid(xdr, arg->stateid);
}