void rds_conn_exit(void)
{
	rds_loop_net_exit(); /* unregister pernet callback */
	rds_loop_exit();

	WARN_ON(!hlist_empty(rds_conn_hash));

	kmem_cache_destroy(rds_conn_slab);

	rds_info_deregister_func(RDS_INFO_CONNECTIONS, rds_conn_info);
	rds_info_deregister_func(RDS_INFO_SEND_MESSAGES,
				 rds_conn_message_info_send);
	rds_info_deregister_func(RDS_INFO_RETRANS_MESSAGES,
				 rds_conn_message_info_retrans);
#if IS_ENABLED(CONFIG_IPV6)
	rds_info_deregister_func(RDS6_INFO_CONNECTIONS, rds6_conn_info);
	rds_info_deregister_func(RDS6_INFO_SEND_MESSAGES,
				 rds6_conn_message_info_send);
	rds_info_deregister_func(RDS6_INFO_RETRANS_MESSAGES,
				 rds6_conn_message_info_retrans);
#endif
}