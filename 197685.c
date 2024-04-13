nfsd4_write(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
	    struct nfsd4_write *write)
{
	stateid_t *stateid = &write->wr_stateid;
	struct file *filp = NULL;
	__be32 status = nfs_ok;
	unsigned long cnt;
	int nvecs;

	if (write->wr_offset >= OFFSET_MAX)
		return nfserr_inval;

	status = nfs4_preprocess_stateid_op(rqstp, cstate, &cstate->current_fh,
						stateid, WR_STATE, &filp, NULL);
	if (status) {
		dprintk("NFSD: nfsd4_write: couldn't process stateid!\n");
		return status;
	}

	cnt = write->wr_buflen;
	write->wr_how_written = write->wr_stable_how;
	gen_boot_verifier(&write->wr_verifier, SVC_NET(rqstp));

	nvecs = fill_in_write_vector(rqstp->rq_vec, write);
	WARN_ON_ONCE(nvecs > ARRAY_SIZE(rqstp->rq_vec));

	status = nfsd_vfs_write(rqstp, &cstate->current_fh, filp,
				write->wr_offset, rqstp->rq_vec, nvecs, &cnt,
				write->wr_how_written);
	fput(filp);

	write->wr_bytes_written = cnt;

	return status;
}