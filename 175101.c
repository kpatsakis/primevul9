int rds_conn_init(void)
{
	int ret;

	ret = rds_loop_net_init(); /* register pernet callback */
	if (ret)
		return ret;

	rds_conn_slab = kmem_cache_create("rds_connection",
					  sizeof(struct rds_connection),
					  0, 0, NULL);
	if (!rds_conn_slab) {
		rds_loop_net_exit();
		return -ENOMEM;
	}

	rds_info_register_func(RDS_INFO_CONNECTIONS, rds_conn_info);
	rds_info_register_func(RDS_INFO_SEND_MESSAGES,
			       rds_conn_message_info_send);
	rds_info_register_func(RDS_INFO_RETRANS_MESSAGES,
			       rds_conn_message_info_retrans);
#if IS_ENABLED(CONFIG_IPV6)
	rds_info_register_func(RDS6_INFO_CONNECTIONS, rds6_conn_info);
	rds_info_register_func(RDS6_INFO_SEND_MESSAGES,
			       rds6_conn_message_info_send);
	rds_info_register_func(RDS6_INFO_RETRANS_MESSAGES,
			       rds6_conn_message_info_retrans);
#endif
	return 0;
}