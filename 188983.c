static void i40e_sync_udp_filters_subtask(struct i40e_pf *pf)
{
	struct i40e_hw *hw = &pf->hw;
	u8 filter_index, type;
	u16 port;
	int i;

	if (!test_and_clear_bit(__I40E_UDP_FILTER_SYNC_PENDING, pf->state))
		return;

	/* acquire RTNL to maintain state of flags and port requests */
	rtnl_lock();

	for (i = 0; i < I40E_MAX_PF_UDP_OFFLOAD_PORTS; i++) {
		if (pf->pending_udp_bitmap & BIT_ULL(i)) {
			struct i40e_udp_port_config *udp_port;
			i40e_status ret = 0;

			udp_port = &pf->udp_ports[i];
			pf->pending_udp_bitmap &= ~BIT_ULL(i);

			port = READ_ONCE(udp_port->port);
			type = READ_ONCE(udp_port->type);
			filter_index = READ_ONCE(udp_port->filter_index);

			/* release RTNL while we wait on AQ command */
			rtnl_unlock();

			if (port)
				ret = i40e_aq_add_udp_tunnel(hw, port,
							     type,
							     &filter_index,
							     NULL);
			else if (filter_index != I40E_UDP_PORT_INDEX_UNUSED)
				ret = i40e_aq_del_udp_tunnel(hw, filter_index,
							     NULL);

			/* reacquire RTNL so we can update filter_index */
			rtnl_lock();

			if (ret) {
				dev_info(&pf->pdev->dev,
					 "%s %s port %d, index %d failed, err %s aq_err %s\n",
					 i40e_tunnel_name(type),
					 port ? "add" : "delete",
					 port,
					 filter_index,
					 i40e_stat_str(&pf->hw, ret),
					 i40e_aq_str(&pf->hw,
						     pf->hw.aq.asq_last_status));
				if (port) {
					/* failed to add, just reset port,
					 * drop pending bit for any deletion
					 */
					udp_port->port = 0;
					pf->pending_udp_bitmap &= ~BIT_ULL(i);
				}
			} else if (port) {
				/* record filter index on success */
				udp_port->filter_index = filter_index;
			}
		}
	}

	rtnl_unlock();
}