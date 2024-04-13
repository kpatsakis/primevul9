nfsd4_secinfo(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
	      union nfsd4_op_u *u)
{
	struct nfsd4_secinfo *secinfo = &u->secinfo;
	struct svc_export *exp;
	struct dentry *dentry;
	__be32 err;

	err = fh_verify(rqstp, &cstate->current_fh, S_IFDIR, NFSD_MAY_EXEC);
	if (err)
		return err;
	err = nfsd_lookup_dentry(rqstp, &cstate->current_fh,
				    secinfo->si_name, secinfo->si_namelen,
				    &exp, &dentry);
	if (err)
		return err;
	fh_unlock(&cstate->current_fh);
	if (d_really_is_negative(dentry)) {
		exp_put(exp);
		err = nfserr_noent;
	} else
		secinfo->si_exp = exp;
	dput(dentry);
	if (cstate->minorversion)
		/* See rfc 5661 section 2.6.3.1.1.8 */
		fh_put(&cstate->current_fh);
	return err;
}