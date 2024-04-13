do_set_nfs4_acl(struct svc_rqst *rqstp, struct svc_fh *fhp,
		struct nfs4_acl *acl, u32 *bmval)
{
	__be32 status;

	status = nfsd4_set_nfs4_acl(rqstp, fhp, acl);
	if (status)
		/*
		 * We should probably fail the whole open at this point,
		 * but we've already created the file, so it's too late;
		 * So this seems the least of evils:
		 */
		bmval[0] &= ~FATTR4_WORD0_ACL;
}