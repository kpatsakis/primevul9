static void encode_opentype(struct xdr_stream *xdr, const struct nfs_openargs *arg)
{
	__be32 *p;

	p = reserve_space(xdr, 4);
	switch (arg->open_flags & O_CREAT) {
	case 0:
		*p = cpu_to_be32(NFS4_OPEN_NOCREATE);
		break;
	default:
		BUG_ON(arg->claim != NFS4_OPEN_CLAIM_NULL);
		*p = cpu_to_be32(NFS4_OPEN_CREATE);
		encode_createmode(xdr, arg);
	}
}