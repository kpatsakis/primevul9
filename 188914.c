static void i40e_udp_tunnel_del(struct net_device *netdev,
				struct udp_tunnel_info *ti)
{
	struct i40e_netdev_priv *np = netdev_priv(netdev);
	struct i40e_vsi *vsi = np->vsi;
	struct i40e_pf *pf = vsi->back;
	u16 port = ntohs(ti->port);
	u8 idx;

	idx = i40e_get_udp_port_idx(pf, port);

	/* Check if port already exists */
	if (idx >= I40E_MAX_PF_UDP_OFFLOAD_PORTS)
		goto not_found;

	switch (ti->type) {
	case UDP_TUNNEL_TYPE_VXLAN:
		if (pf->udp_ports[idx].type != I40E_AQC_TUNNEL_TYPE_VXLAN)
			goto not_found;
		break;
	case UDP_TUNNEL_TYPE_GENEVE:
		if (pf->udp_ports[idx].type != I40E_AQC_TUNNEL_TYPE_NGE)
			goto not_found;
		break;
	default:
		goto not_found;
	}

	/* if port exists, set it to 0 (mark for deletion)
	 * and make it pending
	 */
	pf->udp_ports[idx].port = 0;

	/* Toggle pending bit instead of setting it. This way if we are
	 * deleting a port that has yet to be added we just clear the pending
	 * bit and don't have to worry about it.
	 */
	pf->pending_udp_bitmap ^= BIT_ULL(idx);
	set_bit(__I40E_UDP_FILTER_SYNC_PENDING, pf->state);

	return;
not_found:
	netdev_warn(netdev, "UDP port %d was not found, not deleting\n",
		    port);
}