nfsd4_layoutget(struct svc_rqst *rqstp,
		struct nfsd4_compound_state *cstate, union nfsd4_op_u *u)
{
	struct nfsd4_layoutget *lgp = &u->layoutget;
	struct svc_fh *current_fh = &cstate->current_fh;
	const struct nfsd4_layout_ops *ops;
	struct nfs4_layout_stateid *ls;
	__be32 nfserr;
	int accmode = NFSD_MAY_READ_IF_EXEC;

	switch (lgp->lg_seg.iomode) {
	case IOMODE_READ:
		accmode |= NFSD_MAY_READ;
		break;
	case IOMODE_RW:
		accmode |= NFSD_MAY_READ | NFSD_MAY_WRITE;
		break;
	default:
		dprintk("%s: invalid iomode %d\n",
			__func__, lgp->lg_seg.iomode);
		nfserr = nfserr_badiomode;
		goto out;
	}

	nfserr = fh_verify(rqstp, current_fh, 0, accmode);
	if (nfserr)
		goto out;

	nfserr = nfserr_layoutunavailable;
	ops = nfsd4_layout_verify(current_fh->fh_export, lgp->lg_layout_type);
	if (!ops)
		goto out;

	/*
	 * Verify minlength and range as per RFC5661:
	 *  o  If loga_length is less than loga_minlength,
	 *     the metadata server MUST return NFS4ERR_INVAL.
	 *  o  If the sum of loga_offset and loga_minlength exceeds
	 *     NFS4_UINT64_MAX, and loga_minlength is not
	 *     NFS4_UINT64_MAX, the error NFS4ERR_INVAL MUST result.
	 *  o  If the sum of loga_offset and loga_length exceeds
	 *     NFS4_UINT64_MAX, and loga_length is not NFS4_UINT64_MAX,
	 *     the error NFS4ERR_INVAL MUST result.
	 */
	nfserr = nfserr_inval;
	if (lgp->lg_seg.length < lgp->lg_minlength ||
	    (lgp->lg_minlength != NFS4_MAX_UINT64 &&
	     lgp->lg_minlength > NFS4_MAX_UINT64 - lgp->lg_seg.offset) ||
	    (lgp->lg_seg.length != NFS4_MAX_UINT64 &&
	     lgp->lg_seg.length > NFS4_MAX_UINT64 - lgp->lg_seg.offset))
		goto out;
	if (lgp->lg_seg.length == 0)
		goto out;

	nfserr = nfsd4_preprocess_layout_stateid(rqstp, cstate, &lgp->lg_sid,
						true, lgp->lg_layout_type, &ls);
	if (nfserr) {
		trace_nfsd_layout_get_lookup_fail(&lgp->lg_sid);
		goto out;
	}

	nfserr = nfserr_recallconflict;
	if (atomic_read(&ls->ls_stid.sc_file->fi_lo_recalls))
		goto out_put_stid;

	nfserr = ops->proc_layoutget(d_inode(current_fh->fh_dentry),
				     current_fh, lgp);
	if (nfserr)
		goto out_put_stid;

	nfserr = nfsd4_insert_layout(lgp, ls);

out_put_stid:
	mutex_unlock(&ls->ls_mutex);
	nfs4_put_stid(&ls->ls_stid);
out:
	return nfserr;
}