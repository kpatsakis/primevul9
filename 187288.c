static void encode_test_stateid(struct xdr_stream *xdr,
				struct nfs41_test_stateid_args *args,
				struct compound_hdr *hdr)
{
	encode_op_hdr(xdr, OP_TEST_STATEID, decode_test_stateid_maxsz, hdr);
	encode_uint32(xdr, 1);
	encode_nfs4_stateid(xdr, args->stateid);
}