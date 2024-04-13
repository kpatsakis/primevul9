static void nfs4_open_confirm_done(struct rpc_task *task, void *calldata)
{
	struct nfs4_opendata *data = calldata;

	data->rpc_status = task->tk_status;
	if (data->rpc_status == 0) {
		nfs4_stateid_copy(&data->o_res.stateid, &data->c_res.stateid);
		nfs_confirm_seqid(&data->owner->so_seqid, 0);
		renew_lease(data->o_res.server, data->timestamp);
		data->rpc_done = 1;
	}
}