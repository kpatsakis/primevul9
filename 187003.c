static int nfs4_verify_back_channel_attrs(struct nfs41_create_session_args *args, struct nfs4_session *session)
{
	struct nfs4_channel_attrs *sent = &args->bc_attrs;
	struct nfs4_channel_attrs *rcvd = &session->bc_attrs;

	if (rcvd->max_rqst_sz > sent->max_rqst_sz)
		return -EINVAL;
	if (rcvd->max_resp_sz < sent->max_resp_sz)
		return -EINVAL;
	if (rcvd->max_resp_sz_cached > sent->max_resp_sz_cached)
		return -EINVAL;
	/* These would render the backchannel useless: */
	if (rcvd->max_ops != sent->max_ops)
		return -EINVAL;
	if (rcvd->max_reqs != sent->max_reqs)
		return -EINVAL;
	return 0;
}