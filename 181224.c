nfsd4_savefh(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
	     union nfsd4_op_u *u)
{
	fh_dup2(&cstate->save_fh, &cstate->current_fh);
	if (HAS_STATE_ID(cstate, CURRENT_STATE_ID_FLAG)) {
		memcpy(&cstate->save_stateid, &cstate->current_stateid, sizeof(stateid_t));
		SET_STATE_ID(cstate, SAVED_STATE_ID_FLAG);
	}
	return nfs_ok;
}