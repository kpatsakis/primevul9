static int rds_iw_conn_info_visitor(struct rds_connection *conn,
				    void *buffer)
{
	struct rds_info_rdma_connection *iinfo = buffer;
	struct rds_iw_connection *ic;

	/* We will only ever look at IB transports */
	if (conn->c_trans != &rds_iw_transport)
		return 0;

	iinfo->src_addr = conn->c_laddr;
	iinfo->dst_addr = conn->c_faddr;

	memset(&iinfo->src_gid, 0, sizeof(iinfo->src_gid));
	memset(&iinfo->dst_gid, 0, sizeof(iinfo->dst_gid));
	if (rds_conn_state(conn) == RDS_CONN_UP) {
		struct rds_iw_device *rds_iwdev;
		struct rdma_dev_addr *dev_addr;

		ic = conn->c_transport_data;
		dev_addr = &ic->i_cm_id->route.addr.dev_addr;

		rdma_addr_get_sgid(dev_addr, (union ib_gid *) &iinfo->src_gid);
		rdma_addr_get_dgid(dev_addr, (union ib_gid *) &iinfo->dst_gid);

		rds_iwdev = ib_get_client_data(ic->i_cm_id->device, &rds_iw_client);
		iinfo->max_send_wr = ic->i_send_ring.w_nr;
		iinfo->max_recv_wr = ic->i_recv_ring.w_nr;
		iinfo->max_send_sge = rds_iwdev->max_sge;
		rds_iw_get_mr_info(rds_iwdev, iinfo);
	}
	return 1;
}