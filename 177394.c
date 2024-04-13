static void rds_exit(void)
{
	sock_unregister(rds_family_ops.family);
	proto_unregister(&rds_proto);
	rds_conn_exit();
	rds_cong_exit();
	rds_sysctl_exit();
	rds_threads_exit();
	rds_stats_exit();
	rds_page_exit();
	rds_bind_lock_destroy();
	rds_info_deregister_func(RDS_INFO_SOCKETS, rds_sock_info);
	rds_info_deregister_func(RDS_INFO_RECV_MESSAGES, rds_sock_inc_info);
}