do_open_fhandle(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate, struct nfsd4_open *open)
{
	struct svc_fh *current_fh = &cstate->current_fh;
	__be32 status;
	int accmode = 0;

	/* We don't know the target directory, and therefore can not
	* set the change info
	*/

	memset(&open->op_cinfo, 0, sizeof(struct nfsd4_change_info));

	nfsd4_set_open_owner_reply_cache(cstate, open, current_fh);

	open->op_truncate = (open->op_iattr.ia_valid & ATTR_SIZE) &&
		(open->op_iattr.ia_size == 0);
	/*
	 * In the delegation case, the client is telling us about an
	 * open that it *already* performed locally, some time ago.  We
	 * should let it succeed now if possible.
	 *
	 * In the case of a CLAIM_FH open, on the other hand, the client
	 * may be counting on us to enforce permissions (the Linux 4.1
	 * client uses this for normal opens, for example).
	 */
	if (open->op_claim_type == NFS4_OPEN_CLAIM_DELEG_CUR_FH)
		accmode = NFSD_MAY_OWNER_OVERRIDE;

	status = do_open_permission(rqstp, current_fh, open, accmode);

	return status;
}