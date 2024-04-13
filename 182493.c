SMB2_open_free(struct smb_rqst *rqst)
{
	int i;

	if (rqst && rqst->rq_iov) {
		cifs_small_buf_release(rqst->rq_iov[0].iov_base);
		for (i = 1; i < rqst->rq_nvec; i++)
			if (rqst->rq_iov[i].iov_base != smb2_padding)
				kfree(rqst->rq_iov[i].iov_base);
	}
}