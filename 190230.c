void rds_iw_exit(void)
{
	rds_info_deregister_func(RDS_INFO_IWARP_CONNECTIONS, rds_iw_ic_info);
	rds_iw_destroy_nodev_conns();
	ib_unregister_client(&rds_iw_client);
	rds_iw_sysctl_exit();
	rds_iw_recv_exit();
	rds_trans_unregister(&rds_iw_transport);
}