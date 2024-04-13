bool nfsd4_spo_must_allow(struct svc_rqst *rqstp)
{
	struct nfsd4_compoundres *resp = rqstp->rq_resp;
	struct nfsd4_compoundargs *argp = rqstp->rq_argp;
	struct nfsd4_op *this = &argp->ops[resp->opcnt - 1];
	struct nfsd4_compound_state *cstate = &resp->cstate;
	struct nfs4_op_map *allow = &cstate->clp->cl_spo_must_allow;
	u32 opiter;

	if (!cstate->minorversion)
		return false;

	if (cstate->spo_must_allowed == true)
		return true;

	opiter = resp->opcnt;
	while (opiter < argp->opcnt) {
		this = &argp->ops[opiter++];
		if (test_bit(this->opnum, allow->u.longs) &&
			cstate->clp->cl_mach_cred &&
			nfsd4_mach_creds_match(cstate->clp, rqstp)) {
			cstate->spo_must_allowed = true;
			return true;
		}
	}
	cstate->spo_must_allowed = false;
	return false;
}