static void __bnx2x_add_udp_port(struct bnx2x *bp, u16 port,
				 enum bnx2x_udp_port_type type)
{
	struct bnx2x_udp_tunnel *udp_port = &bp->udp_tunnel_ports[type];

	if (!netif_running(bp->dev) || !IS_PF(bp) || CHIP_IS_E1x(bp))
		return;

	if (udp_port->count && udp_port->dst_port == port) {
		udp_port->count++;
		return;
	}

	if (udp_port->count) {
		DP(BNX2X_MSG_SP,
		   "UDP tunnel [%d] -  destination port limit reached\n",
		   type);
		return;
	}

	udp_port->dst_port = port;
	udp_port->count = 1;
	bnx2x_schedule_sp_rtnl(bp, BNX2X_SP_RTNL_CHANGE_UDP_PORT, 0);
}