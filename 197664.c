nfsd4_remove(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
	     struct nfsd4_remove *remove)
{
	__be32 status;

	if (opens_in_grace(SVC_NET(rqstp)))
		return nfserr_grace;
	status = nfsd_unlink(rqstp, &cstate->current_fh, 0,
			     remove->rm_name, remove->rm_namelen);
	if (!status) {
		fh_unlock(&cstate->current_fh);
		set_change_info(&remove->rm_cinfo, &cstate->current_fh);
	}
	return status;
}