static void __bnx2x_del_udp_port(struct bnx2x *bp, u16 port,
				 enum bnx2x_udp_port_type type)
{
	struct bnx2x_udp_tunnel *udp_port = &bp->udp_tunnel_ports[type];

	if (!IS_PF(bp) || CHIP_IS_E1x(bp))
		return;

	if (!udp_port->count || udp_port->dst_port != port) {
		DP(BNX2X_MSG_SP, "Invalid UDP tunnel [%d] port\n",
		   type);
		return;
	}

	/* Remove reference, and make certain it's no longer in use */
	udp_port->count--;
	if (udp_port->count)
		return;
	udp_port->dst_port = 0;

	if (netif_running(bp->dev))
		bnx2x_schedule_sp_rtnl(bp, BNX2X_SP_RTNL_CHANGE_UDP_PORT, 0);
	else
		DP(BNX2X_MSG_SP, "Deleted UDP tunnel [%d] port %d\n",
		   type, port);
}