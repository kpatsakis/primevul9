nfsd4_restorefh(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
		void *arg)
{
	if (!cstate->save_fh.fh_dentry)
		return nfserr_restorefh;

	fh_dup2(&cstate->current_fh, &cstate->save_fh);
	if (HAS_STATE_ID(cstate, SAVED_STATE_ID_FLAG)) {
		memcpy(&cstate->current_stateid, &cstate->save_stateid, sizeof(stateid_t));
		SET_STATE_ID(cstate, CURRENT_STATE_ID_FLAG);
	}
	return nfs_ok;
}