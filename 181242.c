nfsd4_check_open_attributes(struct svc_rqst *rqstp,
	struct nfsd4_compound_state *cstate, struct nfsd4_open *open)
{
	__be32 status = nfs_ok;

	if (open->op_create == NFS4_OPEN_CREATE) {
		if (open->op_createmode == NFS4_CREATE_UNCHECKED
		    || open->op_createmode == NFS4_CREATE_GUARDED)
			status = check_attr_support(rqstp, cstate,
					open->op_bmval, nfsd_attrmask);
		else if (open->op_createmode == NFS4_CREATE_EXCLUSIVE4_1)
			status = check_attr_support(rqstp, cstate,
					open->op_bmval, nfsd41_ex_attrmask);
	}

	return status;
}