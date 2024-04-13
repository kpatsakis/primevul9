static void encode_free_stateid(struct xdr_stream *xdr,
				struct nfs41_free_stateid_args *args,
				struct compound_hdr *hdr)
{
	encode_op_hdr(xdr, OP_FREE_STATEID, decode_free_stateid_maxsz, hdr);
	encode_nfs4_stateid(xdr, args->stateid);
}