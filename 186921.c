static void encode_getfattr_open(struct xdr_stream *xdr, const u32 *bitmask,
				 const u32 *open_bitmap,
				 struct compound_hdr *hdr)
{
	encode_getattr_three(xdr,
			     bitmask[0] & open_bitmap[0],
			     bitmask[1] & open_bitmap[1],
			     bitmask[2] & open_bitmap[2],
			     hdr);
}