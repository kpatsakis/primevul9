static int nfs4_verify_channel_attrs(struct nfs41_create_session_args *args,
				     struct nfs4_session *session)
{
	int ret;

	ret = nfs4_verify_fore_channel_attrs(args, session);
	if (ret)
		return ret;
	return nfs4_verify_back_channel_attrs(args, session);
}