nfsd4_secinfo_no_name(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
	      struct nfsd4_secinfo_no_name *sin)
{
	__be32 err;

	switch (sin->sin_style) {
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

	sin->sin_exp = exp_get(cstate->current_fh.fh_export);
	fh_put(&cstate->current_fh);
	return nfs_ok;
}