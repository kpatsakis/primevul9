nfsd4_rename(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
	     union nfsd4_op_u *u)
{
	struct nfsd4_rename *rename = &u->rename;
	__be32 status;

	if (opens_in_grace(SVC_NET(rqstp)) &&
		!(cstate->save_fh.fh_export->ex_flags & NFSEXP_NOSUBTREECHECK))
		return nfserr_grace;
	status = nfsd_rename(rqstp, &cstate->save_fh, rename->rn_sname,
			     rename->rn_snamelen, &cstate->current_fh,
			     rename->rn_tname, rename->rn_tnamelen);
	if (status)
		return status;
	set_change_info(&rename->rn_sinfo, &cstate->current_fh);
	set_change_info(&rename->rn_tinfo, &cstate->save_fh);
	return nfs_ok;
}