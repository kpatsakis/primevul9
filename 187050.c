static int nfs4_verify_fore_channel_attrs(struct nfs41_create_session_args *args, struct nfs4_session *session)
{
	struct nfs4_channel_attrs *sent = &args->fc_attrs;
	struct nfs4_channel_attrs *rcvd = &session->fc_attrs;

	if (rcvd->max_resp_sz > sent->max_resp_sz)
		return -EINVAL;
	/*
	 * Our requested max_ops is the minimum we need; we're not
	 * prepared to break up compounds into smaller pieces than that.
	 * So, no point even trying to continue if the server won't
	 * cooperate:
	 */
	if (rcvd->max_ops < sent->max_ops)
		return -EINVAL;
	if (rcvd->max_reqs == 0)
		return -EINVAL;
	if (rcvd->max_reqs > NFS4_MAX_SLOT_TABLE)
		rcvd->max_reqs = NFS4_MAX_SLOT_TABLE;
	return 0;
}