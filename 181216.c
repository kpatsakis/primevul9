nfsd4_layoutreturn(struct svc_rqst *rqstp,
		struct nfsd4_compound_state *cstate, union nfsd4_op_u *u)
{
	struct nfsd4_layoutreturn *lrp = &u->layoutreturn;
	struct svc_fh *current_fh = &cstate->current_fh;
	__be32 nfserr;

	nfserr = fh_verify(rqstp, current_fh, 0, NFSD_MAY_NOP);
	if (nfserr)
		goto out;

	nfserr = nfserr_layoutunavailable;
	if (!nfsd4_layout_verify(current_fh->fh_export, lrp->lr_layout_type))
		goto out;

	switch (lrp->lr_seg.iomode) {
	case IOMODE_READ:
	case IOMODE_RW:
	case IOMODE_ANY:
		break;
	default:
		dprintk("%s: invalid iomode %d\n", __func__,
			lrp->lr_seg.iomode);
		nfserr = nfserr_inval;
		goto out;
	}

	switch (lrp->lr_return_type) {
	case RETURN_FILE:
		nfserr = nfsd4_return_file_layouts(rqstp, cstate, lrp);
		break;
	case RETURN_FSID:
	case RETURN_ALL:
		nfserr = nfsd4_return_client_layouts(rqstp, cstate, lrp);
		break;
	default:
		dprintk("%s: invalid return_type %d\n", __func__,
			lrp->lr_return_type);
		nfserr = nfserr_inval;
		break;
	}
out:
	return nfserr;
}