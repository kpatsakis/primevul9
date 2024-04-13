nfsd4_secinfo_no_name(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
		union nfsd4_op_u *u)
{
	__be32 err;

	switch (u->secinfo_no_name.sin_style) {
	case NFS4_SECINFO_STYLE4_CURRENT_FH:
		break;
	case NFS4_SECINFO_STYLE4_PARENT:
		err = nfsd4_do_lookupp(rqstp, &cstate->current_fh);
		if (err)
			return err;
		break;
	default:
		return nfserr_inval;
	}

	u->secinfo_no_name.sin_exp = exp_get(cstate->current_fh.fh_export);
	fh_put(&cstate->current_fh);
	return nfs_ok;
}