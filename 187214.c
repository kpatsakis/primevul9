static void encode_share_access(struct xdr_stream *xdr, fmode_t fmode)
{
	__be32 *p;

	p = reserve_space(xdr, 8);
	switch (fmode & (FMODE_READ|FMODE_WRITE)) {
	case FMODE_READ:
		*p++ = cpu_to_be32(NFS4_SHARE_ACCESS_READ);
		break;
	case FMODE_WRITE:
		*p++ = cpu_to_be32(NFS4_SHARE_ACCESS_WRITE);
		break;
	case FMODE_READ|FMODE_WRITE:
		*p++ = cpu_to_be32(NFS4_SHARE_ACCESS_BOTH);
		break;
	default:
		*p++ = cpu_to_be32(0);
	}
	*p = cpu_to_be32(0);		/* for linux, share_deny = 0 always */
}