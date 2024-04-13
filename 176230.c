static int ax88179_change_mtu(struct net_device *net, int new_mtu)
{
	struct usbnet *dev = netdev_priv(net);
	u16 tmp16;

	net->mtu = new_mtu;
	dev->hard_mtu = net->mtu + net->hard_header_len;

	if (net->mtu > 1500) {
		ax88179_read_cmd(dev, AX_ACCESS_MAC, AX_MEDIUM_STATUS_MODE,
				 2, 2, &tmp16);
		tmp16 |= AX_MEDIUM_JUMBO_EN;
		ax88179_write_cmd(dev, AX_ACCESS_MAC, AX_MEDIUM_STATUS_MODE,
				  2, 2, &tmp16);
	} else {
		ax88179_read_cmd(dev, AX_ACCESS_MAC, AX_MEDIUM_STATUS_MODE,
				 2, 2, &tmp16);
		tmp16 &= ~AX_MEDIUM_JUMBO_EN;
		ax88179_write_cmd(dev, AX_ACCESS_MAC, AX_MEDIUM_STATUS_MODE,
				  2, 2, &tmp16);
	}

	/* max qlen depend on hard_mtu and rx_urb_size */
	usbnet_update_max_qlen(dev);

	return 0;
}