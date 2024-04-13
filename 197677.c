nfsd4_layoutcommit(struct svc_rqst *rqstp,
		struct nfsd4_compound_state *cstate,
		struct nfsd4_layoutcommit *lcp)
{
	const struct nfsd4_layout_seg *seg = &lcp->lc_seg;
	struct svc_fh *current_fh = &cstate->current_fh;
	const struct nfsd4_layout_ops *ops;
	loff_t new_size = lcp->lc_last_wr + 1;
	struct inode *inode;
	struct nfs4_layout_stateid *ls;
	__be32 nfserr;

	nfserr = fh_verify(rqstp, current_fh, 0, NFSD_MAY_WRITE);
	if (nfserr)
		goto out;

	nfserr = nfserr_layoutunavailable;
	ops = nfsd4_layout_verify(current_fh->fh_export, lcp->lc_layout_type);
	if (!ops)
		goto out;
	inode = d_inode(current_fh->fh_dentry);

	nfserr = nfserr_inval;
	if (new_size <= seg->offset) {
		dprintk("pnfsd: last write before layout segment\n");
		goto out;
	}
	if (new_size > seg->offset + seg->length) {
		dprintk("pnfsd: last write beyond layout segment\n");
		goto out;
	}
	if (!lcp->lc_newoffset && new_size > i_size_read(inode)) {
		dprintk("pnfsd: layoutcommit beyond EOF\n");
		goto out;
	}

	nfserr = nfsd4_preprocess_layout_stateid(rqstp, cstate, &lcp->lc_sid,
						false, lcp->lc_layout_type,
						&ls);
	if (nfserr) {
		trace_layout_commit_lookup_fail(&lcp->lc_sid);
		/* fixup error code as per RFC5661 */
		if (nfserr == nfserr_bad_stateid)
			nfserr = nfserr_badlayout;
		goto out;
	}

	/* LAYOUTCOMMIT does not require any serialization */
	mutex_unlock(&ls->ls_mutex);

	if (new_size > i_size_read(inode)) {
		lcp->lc_size_chg = 1;
		lcp->lc_newsize = new_size;
	} else {
		lcp->lc_size_chg = 0;
	}

	nfserr = ops->proc_layoutcommit(inode, lcp);
	nfs4_put_stid(&ls->ls_stid);
out:
	return nfserr;
}