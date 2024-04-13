int nfs4_setup_sequence(const struct nfs_server *server,
			struct nfs4_sequence_args *args,
			struct nfs4_sequence_res *res,
			struct rpc_task *task)
{
	struct nfs4_session *session = nfs4_get_session(server);
	int ret = 0;

	if (session == NULL)
		goto out;

	dprintk("--> %s clp %p session %p sr_slot %td\n",
		__func__, session->clp, session, res->sr_slot ?
			res->sr_slot - session->fc_slot_table.slots : -1);

	ret = nfs41_setup_sequence(session, args, res, task);
out:
	dprintk("<-- %s status=%d\n", __func__, ret);
	return ret;
}