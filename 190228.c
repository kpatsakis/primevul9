int rds_iw_init(void)
{
	int ret;

	INIT_LIST_HEAD(&rds_iw_devices);

	ret = ib_register_client(&rds_iw_client);
	if (ret)
		goto out;

	ret = rds_iw_sysctl_init();
	if (ret)
		goto out_ibreg;

	ret = rds_iw_recv_init();
	if (ret)
		goto out_sysctl;

	ret = rds_trans_register(&rds_iw_transport);
	if (ret)
		goto out_recv;

	rds_info_register_func(RDS_INFO_IWARP_CONNECTIONS, rds_iw_ic_info);

	goto out;

out_recv:
	rds_iw_recv_exit();
out_sysctl:
	rds_iw_sysctl_exit();
out_ibreg:
	ib_unregister_client(&rds_iw_client);
out:
	return ret;
}