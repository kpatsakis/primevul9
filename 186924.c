static void encode_fs_locations(struct xdr_stream *xdr, const u32* bitmask, struct compound_hdr *hdr)
{
	encode_getattr_two(xdr, bitmask[0] & nfs4_fs_locations_bitmap[0],
			   bitmask[1] & nfs4_fs_locations_bitmap[1], hdr);
}