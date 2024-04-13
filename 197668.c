nfsd4_putfh(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
	    struct nfsd4_putfh *putfh)
{
	fh_put(&cstate->current_fh);
	cstate->current_fh.fh_handle.fh_size = putfh->pf_fhlen;
	memcpy(&cstate->current_fh.fh_handle.fh_base, putfh->pf_fhval,
	       putfh->pf_fhlen);
	return fh_verify(rqstp, &cstate->current_fh, 0, NFSD_MAY_BYPASS_GSS);
}