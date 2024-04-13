SMB2_set_info_free(struct smb_rqst *rqst)
{
	if (rqst && rqst->rq_iov)
		cifs_buf_release(rqst->rq_iov[0].iov_base); /* request */
}