encode_setacl(struct xdr_stream *xdr, struct nfs_setaclargs *arg, struct compound_hdr *hdr)
{
	__be32 *p;

	encode_op_hdr(xdr, OP_SETATTR, decode_setacl_maxsz, hdr);
	encode_nfs4_stateid(xdr, &zero_stateid);
	p = reserve_space(xdr, 2*4);
	*p++ = cpu_to_be32(1);
	*p = cpu_to_be32(FATTR4_WORD0_ACL);
	BUG_ON(arg->acl_len % 4);
	p = reserve_space(xdr, 4);
	*p = cpu_to_be32(arg->acl_len);
	xdr_write_pages(xdr, arg->acl_pages, arg->acl_pgbase, arg->acl_len);
}