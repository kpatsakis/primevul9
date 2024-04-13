static void encode_fsinfo(struct xdr_stream *xdr, const u32* bitmask, struct compound_hdr *hdr)
{
	encode_getattr_three(xdr,
			     bitmask[0] & nfs4_fsinfo_bitmap[0],
			     bitmask[1] & nfs4_fsinfo_bitmap[1],
			     bitmask[2] & nfs4_fsinfo_bitmap[2],
			     hdr);
}