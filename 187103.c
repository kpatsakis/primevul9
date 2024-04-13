static void encode_create(struct xdr_stream *xdr, const struct nfs4_create_arg *create, struct compound_hdr *hdr)
{
	__be32 *p;

	encode_op_hdr(xdr, OP_CREATE, decode_create_maxsz, hdr);
	encode_uint32(xdr, create->ftype);

	switch (create->ftype) {
	case NF4LNK:
		p = reserve_space(xdr, 4);
		*p = cpu_to_be32(create->u.symlink.len);
		xdr_write_pages(xdr, create->u.symlink.pages, 0, create->u.symlink.len);
		break;

	case NF4BLK: case NF4CHR:
		p = reserve_space(xdr, 8);
		*p++ = cpu_to_be32(create->u.device.specdata1);
		*p = cpu_to_be32(create->u.device.specdata2);
		break;

	default:
		break;
	}

	encode_string(xdr, create->name->len, create->name->name);
	encode_attrs(xdr, create->attrs, create->server);
}