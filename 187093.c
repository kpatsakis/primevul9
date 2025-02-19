static int nfs4_read_done(struct rpc_task *task, struct nfs_read_data *data)
{

	dprintk("--> %s\n", __func__);

	if (!nfs4_sequence_done(task, &data->res.seq_res))
		return -EAGAIN;

	return data->read_done_cb ? data->read_done_cb(task, data) :
				    nfs4_read_done_cb(task, data);
}