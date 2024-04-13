nfsd4_write(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
	    union nfsd4_op_u *u)
{
	struct nfsd4_write *write = &u->write;
	stateid_t *stateid = &write->wr_stateid;
	struct file *filp = NULL;
	__be32 status = nfs_ok;
	unsigned long cnt;
	int nvecs;

	if (write->wr_offset >= OFFSET_MAX)
		return nfserr_inval;

	cnt = write->wr_buflen;
	trace_nfsd_write_start(rqstp, &cstate->current_fh,
			       write->wr_offset, cnt);
	status = nfs4_preprocess_stateid_op(rqstp, cstate, &cstate->current_fh,
						stateid, WR_STATE, &filp, NULL);
	if (status) {
		dprintk("NFSD: nfsd4_write: couldn't process stateid!\n");
		return status;
	}

	write->wr_how_written = write->wr_stable_how;
	gen_boot_verifier(&write->wr_verifier, SVC_NET(rqstp));

	nvecs = svc_fill_write_vector(rqstp, write->wr_pagelist,
				      &write->wr_head, write->wr_buflen);
	if (!nvecs)
		return nfserr_io;
	WARN_ON_ONCE(nvecs > ARRAY_SIZE(rqstp->rq_vec));

	status = nfsd_vfs_write(rqstp, &cstate->current_fh, filp,
				write->wr_offset, rqstp->rq_vec, nvecs, &cnt,
				write->wr_how_written);
	fput(filp);

	write->wr_bytes_written = cnt;
	trace_nfsd_write_done(rqstp, &cstate->current_fh,
			      write->wr_offset, cnt);
	return status;
}