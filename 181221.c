nfsd4_read(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
	   union nfsd4_op_u *u)
{
	struct nfsd4_read *read = &u->read;
	__be32 status;

	read->rd_filp = NULL;
	if (read->rd_offset >= OFFSET_MAX)
		return nfserr_inval;

	trace_nfsd_read_start(rqstp, &cstate->current_fh,
			      read->rd_offset, read->rd_length);

	/*
	 * If we do a zero copy read, then a client will see read data
	 * that reflects the state of the file *after* performing the
	 * following compound.
	 *
	 * To ensure proper ordering, we therefore turn off zero copy if
	 * the client wants us to do more in this compound:
	 */
	if (!nfsd4_last_compound_op(rqstp))
		clear_bit(RQ_SPLICE_OK, &rqstp->rq_flags);

	/* check stateid */
	status = nfs4_preprocess_stateid_op(rqstp, cstate, &cstate->current_fh,
					&read->rd_stateid, RD_STATE,
					&read->rd_filp, &read->rd_tmp_file);
	if (status) {
		dprintk("NFSD: nfsd4_read: couldn't process stateid!\n");
		goto out;
	}
	status = nfs_ok;
out:
	read->rd_rqstp = rqstp;
	read->rd_fhp = &cstate->current_fh;
	return status;
}