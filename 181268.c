nfsd4_setattr(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
	      union nfsd4_op_u *u)
{
	struct nfsd4_setattr *setattr = &u->setattr;
	__be32 status = nfs_ok;
	int err;

	if (setattr->sa_iattr.ia_valid & ATTR_SIZE) {
		status = nfs4_preprocess_stateid_op(rqstp, cstate,
				&cstate->current_fh, &setattr->sa_stateid,
				WR_STATE, NULL, NULL);
		if (status) {
			dprintk("NFSD: nfsd4_setattr: couldn't process stateid!\n");
			return status;
		}
	}
	err = fh_want_write(&cstate->current_fh);
	if (err)
		return nfserrno(err);
	status = nfs_ok;

	status = check_attr_support(rqstp, cstate, setattr->sa_bmval,
				    nfsd_attrmask);
	if (status)
		goto out;

	if (setattr->sa_acl != NULL)
		status = nfsd4_set_nfs4_acl(rqstp, &cstate->current_fh,
					    setattr->sa_acl);
	if (status)
		goto out;
	if (setattr->sa_label.len)
		status = nfsd4_set_nfs4_label(rqstp, &cstate->current_fh,
				&setattr->sa_label);
	if (status)
		goto out;
	status = nfsd_setattr(rqstp, &cstate->current_fh, &setattr->sa_iattr,
				0, (time_t)0);
out:
	fh_drop_write(&cstate->current_fh);
	return status;
}