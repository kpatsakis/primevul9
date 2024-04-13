static inline void encode_createmode(struct xdr_stream *xdr, const struct nfs_openargs *arg)
{
	__be32 *p;
	struct nfs_client *clp;

	p = reserve_space(xdr, 4);
	switch(arg->open_flags & O_EXCL) {
	case 0:
		*p = cpu_to_be32(NFS4_CREATE_UNCHECKED);
		encode_attrs(xdr, arg->u.attrs, arg->server);
		break;
	default:
		clp = arg->server->nfs_client;
		if (clp->cl_mvops->minor_version > 0) {
			if (nfs4_has_persistent_session(clp)) {
				*p = cpu_to_be32(NFS4_CREATE_GUARDED);
				encode_attrs(xdr, arg->u.attrs, arg->server);
			} else {
				struct iattr dummy;

				*p = cpu_to_be32(NFS4_CREATE_EXCLUSIVE4_1);
				encode_nfs4_verifier(xdr, &arg->u.verifier);
				dummy.ia_valid = 0;
				encode_attrs(xdr, &dummy, arg->server);
			}
		} else {
			*p = cpu_to_be32(NFS4_CREATE_EXCLUSIVE);
			encode_nfs4_verifier(xdr, &arg->u.verifier);
		}
	}
}