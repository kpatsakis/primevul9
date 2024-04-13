nfsd4_getfh(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
	    struct svc_fh **getfh)
{
	if (!cstate->current_fh.fh_dentry)
		return nfserr_nofilehandle;

	*getfh = &cstate->current_fh;
	return nfs_ok;
}