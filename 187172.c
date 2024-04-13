static void encode_write(struct xdr_stream *xdr, const struct nfs_writeargs *args, struct compound_hdr *hdr)
{
	__be32 *p;

	encode_op_hdr(xdr, OP_WRITE, decode_write_maxsz, hdr);
	encode_open_stateid(xdr, args->context, args->lock_context,
			FMODE_WRITE, hdr->minorversion);

	p = reserve_space(xdr, 16);
	p = xdr_encode_hyper(p, args->offset);
	*p++ = cpu_to_be32(args->stable);
	*p = cpu_to_be32(args->count);

	xdr_write_pages(xdr, args->pages, args->pgbase, args->count);
}