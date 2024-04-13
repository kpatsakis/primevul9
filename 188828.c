static void i40e_udp_tunnel_add(struct net_device *netdev,
				struct udp_tunnel_info *ti)
{
	struct i40e_netdev_priv *np = netdev_priv(netdev);
	struct i40e_vsi *vsi = np->vsi;
	struct i40e_pf *pf = vsi->back;
	u16 port = ntohs(ti->port);
	u8 next_idx;
	u8 idx;

	idx = i40e_get_udp_port_idx(pf, port);

	/* Check if port already exists */
	if (idx < I40E_MAX_PF_UDP_OFFLOAD_PORTS) {
		netdev_info(netdev, "port %d already offloaded\n", port);
		return;
	}

	/* Now check if there is space to add the new port */
	next_idx = i40e_get_udp_port_idx(pf, 0);

	if (next_idx == I40E_MAX_PF_UDP_OFFLOAD_PORTS) {
		netdev_info(netdev, "maximum number of offloaded UDP ports reached, not adding port %d\n",
			    port);
		return;
	}

	switch (ti->type) {
	case UDP_TUNNEL_TYPE_VXLAN:
		pf->udp_ports[next_idx].type = I40E_AQC_TUNNEL_TYPE_VXLAN;
		break;
	case UDP_TUNNEL_TYPE_GENEVE:
		if (!(pf->hw_features & I40E_HW_GENEVE_OFFLOAD_CAPABLE))
			return;
		pf->udp_ports[next_idx].type = I40E_AQC_TUNNEL_TYPE_NGE;
		break;
	default:
		return;
	}

	/* New port: add it and mark its index in the bitmap */
	pf->udp_ports[next_idx].port = port;
	pf->udp_ports[next_idx].filter_index = I40E_UDP_PORT_INDEX_UNUSED;
	pf->pending_udp_bitmap |= BIT_ULL(next_idx);
	set_bit(__I40E_UDP_FILTER_SYNC_PENDING, pf->state);
}