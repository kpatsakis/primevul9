do_open_permission(struct svc_rqst *rqstp, struct svc_fh *current_fh, struct nfsd4_open *open, int accmode)
{
	__be32 status;

	if (open->op_truncate &&
		!(open->op_share_access & NFS4_SHARE_ACCESS_WRITE))
		return nfserr_inval;

	accmode |= NFSD_MAY_READ_IF_EXEC;

	if (open->op_share_access & NFS4_SHARE_ACCESS_READ)
		accmode |= NFSD_MAY_READ;
	if (open->op_share_access & NFS4_SHARE_ACCESS_WRITE)
		accmode |= (NFSD_MAY_WRITE | NFSD_MAY_TRUNC);
	if (open->op_share_deny & NFS4_SHARE_DENY_READ)
		accmode |= NFSD_MAY_WRITE;

	status = fh_verify(rqstp, current_fh, S_IFREG, accmode);

	return status;
}