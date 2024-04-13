static void bnx2x_udp_tunnel_del(struct net_device *netdev,
				 struct udp_tunnel_info *ti)
{
	struct bnx2x *bp = netdev_priv(netdev);
	u16 t_port = ntohs(ti->port);

	switch (ti->type) {
	case UDP_TUNNEL_TYPE_VXLAN:
		__bnx2x_del_udp_port(bp, t_port, BNX2X_UDP_PORT_VXLAN);
		break;
	case UDP_TUNNEL_TYPE_GENEVE:
		__bnx2x_del_udp_port(bp, t_port, BNX2X_UDP_PORT_GENEVE);
		break;
	default:
		break;
	}
}