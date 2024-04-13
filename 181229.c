nfsd4_fallocate(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
		struct nfsd4_fallocate *fallocate, int flags)
{
	__be32 status = nfserr_notsupp;
	struct file *file;

	status = nfs4_preprocess_stateid_op(rqstp, cstate, &cstate->current_fh,
					    &fallocate->falloc_stateid,
					    WR_STATE, &file, NULL);
	if (status != nfs_ok) {
		dprintk("NFSD: nfsd4_fallocate: couldn't process stateid!\n");
		return status;
	}

	status = nfsd4_vfs_fallocate(rqstp, &cstate->current_fh, file,
				     fallocate->falloc_offset,
				     fallocate->falloc_length,
				     flags);
	fput(file);
	return status;
}