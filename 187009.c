static void nfs4_xdr_enc_setclientid_confirm(struct rpc_rqst *req,
					     struct xdr_stream *xdr,
					     struct nfs4_setclientid_res *arg)
{
	struct compound_hdr hdr = {
		.nops	= 0,
	};
	const u32 lease_bitmap[3] = { FATTR4_WORD0_LEASE_TIME };

	encode_compound_hdr(xdr, req, &hdr);
	encode_setclientid_confirm(xdr, arg, &hdr);
	encode_putrootfh(xdr, &hdr);
	encode_fsinfo(xdr, lease_bitmap, &hdr);
	encode_nops(&hdr);
}