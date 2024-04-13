nfsd4_commit(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
	     struct nfsd4_commit *commit)
{
	gen_boot_verifier(&commit->co_verf, SVC_NET(rqstp));
	return nfsd_commit(rqstp, &cstate->current_fh, commit->co_offset,
			     commit->co_count);
}