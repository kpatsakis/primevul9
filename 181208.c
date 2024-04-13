check_attr_support(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
		   u32 *bmval, u32 *writable)
{
	struct dentry *dentry = cstate->current_fh.fh_dentry;
	struct svc_export *exp = cstate->current_fh.fh_export;

	if (!nfsd_attrs_supported(cstate->minorversion, bmval))
		return nfserr_attrnotsupp;
	if ((bmval[0] & FATTR4_WORD0_ACL) && !IS_POSIXACL(d_inode(dentry)))
		return nfserr_attrnotsupp;
	if ((bmval[2] & FATTR4_WORD2_SECURITY_LABEL) &&
			!(exp->ex_flags & NFSEXP_SECURITY_LABEL))
		return nfserr_attrnotsupp;
	if (writable && !bmval_is_subset(bmval, writable))
		return nfserr_inval;
	if (writable && (bmval[2] & FATTR4_WORD2_MODE_UMASK) &&
			(bmval[1] & FATTR4_WORD1_MODE))
		return nfserr_inval;
	return nfs_ok;
}