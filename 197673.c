nfsd4_lookup(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
	     struct nfsd4_lookup *lookup)
{
	return nfsd_lookup(rqstp, &cstate->current_fh,
			   lookup->lo_name, lookup->lo_len,
			   &cstate->current_fh);
}