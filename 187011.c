static void encode_reclaim_complete(struct xdr_stream *xdr,
				    struct nfs41_reclaim_complete_args *args,
				    struct compound_hdr *hdr)
{
	encode_op_hdr(xdr, OP_RECLAIM_COMPLETE, decode_reclaim_complete_maxsz, hdr);
	encode_uint32(xdr, args->one_fs);
}