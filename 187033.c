static int nfs4_sequence_done(struct rpc_task *task,
			       struct nfs4_sequence_res *res)
{
	if (res->sr_session == NULL)
		return 1;
	return nfs41_sequence_done(task, res);
}