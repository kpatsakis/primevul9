do_open_lookup(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate, struct nfsd4_open *open, struct svc_fh **resfh)
{
	struct svc_fh *current_fh = &cstate->current_fh;
	int accmode;
	__be32 status;

	*resfh = kmalloc(sizeof(struct svc_fh), GFP_KERNEL);
	if (!*resfh)
		return nfserr_jukebox;
	fh_init(*resfh, NFS4_FHSIZE);
	open->op_truncate = 0;

	if (open->op_create) {
		/* FIXME: check session persistence and pnfs flags.
		 * The nfsv4.1 spec requires the following semantics:
		 *
		 * Persistent   | pNFS   | Server REQUIRED | Client Allowed
		 * Reply Cache  | server |                 |
		 * -------------+--------+-----------------+--------------------
		 * no           | no     | EXCLUSIVE4_1    | EXCLUSIVE4_1
		 *              |        |                 | (SHOULD)
		 *              |        | and EXCLUSIVE4  | or EXCLUSIVE4
		 *              |        |                 | (SHOULD NOT)
		 * no           | yes    | EXCLUSIVE4_1    | EXCLUSIVE4_1
		 * yes          | no     | GUARDED4        | GUARDED4
		 * yes          | yes    | GUARDED4        | GUARDED4
		 */

		/*
		 * Note: create modes (UNCHECKED,GUARDED...) are the same
		 * in NFSv4 as in v3 except EXCLUSIVE4_1.
		 */
		current->fs->umask = open->op_umask;
		status = do_nfsd_create(rqstp, current_fh, open->op_fname.data,
					open->op_fname.len, &open->op_iattr,
					*resfh, open->op_createmode,
					(u32 *)open->op_verf.data,
					&open->op_truncate, &open->op_created);
		current->fs->umask = 0;

		if (!status && open->op_label.len)
			nfsd4_security_inode_setsecctx(*resfh, &open->op_label, open->op_bmval);

		/*
		 * Following rfc 3530 14.2.16, and rfc 5661 18.16.4
		 * use the returned bitmask to indicate which attributes
		 * we used to store the verifier:
		 */
		if (nfsd_create_is_exclusive(open->op_createmode) && status == 0)
			open->op_bmval[1] |= (FATTR4_WORD1_TIME_ACCESS |
						FATTR4_WORD1_TIME_MODIFY);
	} else
		/*
		 * Note this may exit with the parent still locked.
		 * We will hold the lock until nfsd4_open's final
		 * lookup, to prevent renames or unlinks until we've had
		 * a chance to an acquire a delegation if appropriate.
		 */
		status = nfsd_lookup(rqstp, current_fh,
				     open->op_fname.data, open->op_fname.len, *resfh);
	if (status)
		goto out;
	status = nfsd_check_obj_isreg(*resfh);
	if (status)
		goto out;

	if (is_create_with_attrs(open) && open->op_acl != NULL)
		do_set_nfs4_acl(rqstp, *resfh, open->op_acl, open->op_bmval);

	nfsd4_set_open_owner_reply_cache(cstate, open, *resfh);
	accmode = NFSD_MAY_NOP;
	if (open->op_created ||
			open->op_claim_type == NFS4_OPEN_CLAIM_DELEGATE_CUR)
		accmode |= NFSD_MAY_OWNER_OVERRIDE;
	status = do_open_permission(rqstp, *resfh, open, accmode);
	set_change_info(&open->op_cinfo, current_fh);
out:
	return status;
}