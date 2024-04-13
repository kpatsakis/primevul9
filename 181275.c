nfsd4_getattr(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
	      union nfsd4_op_u *u)
{
	struct nfsd4_getattr *getattr = &u->getattr;
	__be32 status;

	status = fh_verify(rqstp, &cstate->current_fh, 0, NFSD_MAY_NOP);
	if (status)
		return status;

	if (getattr->ga_bmval[1] & NFSD_WRITEONLY_ATTRS_WORD1)
		return nfserr_inval;

	getattr->ga_bmval[0] &= nfsd_suppattrs[cstate->minorversion][0];
	getattr->ga_bmval[1] &= nfsd_suppattrs[cstate->minorversion][1];
	getattr->ga_bmval[2] &= nfsd_suppattrs[cstate->minorversion][2];

	getattr->ga_fhp = &cstate->current_fh;
	return nfs_ok;
}