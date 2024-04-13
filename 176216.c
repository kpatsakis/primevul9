ax88179_set_features(struct net_device *net, netdev_features_t features)
{
	u8 tmp;
	struct usbnet *dev = netdev_priv(net);
	netdev_features_t changed = net->features ^ features;

	if (changed & NETIF_F_IP_CSUM) {
		ax88179_read_cmd(dev, AX_ACCESS_MAC, AX_TXCOE_CTL, 1, 1, &tmp);
		tmp ^= AX_TXCOE_TCP | AX_TXCOE_UDP;
		ax88179_write_cmd(dev, AX_ACCESS_MAC, AX_TXCOE_CTL, 1, 1, &tmp);
	}

	if (changed & NETIF_F_IPV6_CSUM) {
		ax88179_read_cmd(dev, AX_ACCESS_MAC, AX_TXCOE_CTL, 1, 1, &tmp);
		tmp ^= AX_TXCOE_TCPV6 | AX_TXCOE_UDPV6;
		ax88179_write_cmd(dev, AX_ACCESS_MAC, AX_TXCOE_CTL, 1, 1, &tmp);
	}

	if (changed & NETIF_F_RXCSUM) {
		ax88179_read_cmd(dev, AX_ACCESS_MAC, AX_RXCOE_CTL, 1, 1, &tmp);
		tmp ^= AX_RXCOE_IP | AX_RXCOE_TCP | AX_RXCOE_UDP |
		       AX_RXCOE_TCPV6 | AX_RXCOE_UDPV6;
		ax88179_write_cmd(dev, AX_ACCESS_MAC, AX_RXCOE_CTL, 1, 1, &tmp);
	}

	return 0;
}