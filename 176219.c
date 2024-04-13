static int ax88179_bind(struct usbnet *dev, struct usb_interface *intf)
{
	u8 buf[5];
	u16 *tmp16;
	u8 *tmp;
	struct ax88179_data *ax179_data = (struct ax88179_data *)dev->data;
	struct ethtool_eee eee_data;

	usbnet_get_endpoints(dev, intf);

	tmp16 = (u16 *)buf;
	tmp = (u8 *)buf;

	memset(ax179_data, 0, sizeof(*ax179_data));

	/* Power up ethernet PHY */
	*tmp16 = 0;
	ax88179_write_cmd(dev, AX_ACCESS_MAC, AX_PHYPWR_RSTCTL, 2, 2, tmp16);
	*tmp16 = AX_PHYPWR_RSTCTL_IPRL;
	ax88179_write_cmd(dev, AX_ACCESS_MAC, AX_PHYPWR_RSTCTL, 2, 2, tmp16);
	msleep(200);

	*tmp = AX_CLK_SELECT_ACS | AX_CLK_SELECT_BCS;
	ax88179_write_cmd(dev, AX_ACCESS_MAC, AX_CLK_SELECT, 1, 1, tmp);
	msleep(100);

	/* Read MAC address from DTB or asix chip */
	ax88179_get_mac_addr(dev);
	memcpy(dev->net->perm_addr, dev->net->dev_addr, ETH_ALEN);

	/* RX bulk configuration */
	memcpy(tmp, &AX88179_BULKIN_SIZE[0], 5);
	ax88179_write_cmd(dev, AX_ACCESS_MAC, AX_RX_BULKIN_QCTRL, 5, 5, tmp);

	dev->rx_urb_size = 1024 * 20;

	*tmp = 0x34;
	ax88179_write_cmd(dev, AX_ACCESS_MAC, AX_PAUSE_WATERLVL_LOW, 1, 1, tmp);

	*tmp = 0x52;
	ax88179_write_cmd(dev, AX_ACCESS_MAC, AX_PAUSE_WATERLVL_HIGH,
			  1, 1, tmp);

	dev->net->netdev_ops = &ax88179_netdev_ops;
	dev->net->ethtool_ops = &ax88179_ethtool_ops;
	dev->net->needed_headroom = 8;
	dev->net->max_mtu = 4088;

	/* Initialize MII structure */
	dev->mii.dev = dev->net;
	dev->mii.mdio_read = ax88179_mdio_read;
	dev->mii.mdio_write = ax88179_mdio_write;
	dev->mii.phy_id_mask = 0xff;
	dev->mii.reg_num_mask = 0xff;
	dev->mii.phy_id = 0x03;
	dev->mii.supports_gmii = 1;

	dev->net->features |= NETIF_F_SG | NETIF_F_IP_CSUM |
			      NETIF_F_IPV6_CSUM | NETIF_F_RXCSUM | NETIF_F_TSO;

	dev->net->hw_features |= dev->net->features;

	netif_set_gso_max_size(dev->net, 16384);

	/* Enable checksum offload */
	*tmp = AX_RXCOE_IP | AX_RXCOE_TCP | AX_RXCOE_UDP |
	       AX_RXCOE_TCPV6 | AX_RXCOE_UDPV6;
	ax88179_write_cmd(dev, AX_ACCESS_MAC, AX_RXCOE_CTL, 1, 1, tmp);

	*tmp = AX_TXCOE_IP | AX_TXCOE_TCP | AX_TXCOE_UDP |
	       AX_TXCOE_TCPV6 | AX_TXCOE_UDPV6;
	ax88179_write_cmd(dev, AX_ACCESS_MAC, AX_TXCOE_CTL, 1, 1, tmp);

	/* Configure RX control register => start operation */
	*tmp16 = AX_RX_CTL_DROPCRCERR | AX_RX_CTL_IPE | AX_RX_CTL_START |
		 AX_RX_CTL_AP | AX_RX_CTL_AMALL | AX_RX_CTL_AB;
	ax88179_write_cmd(dev, AX_ACCESS_MAC, AX_RX_CTL, 2, 2, tmp16);

	*tmp = AX_MONITOR_MODE_PMETYPE | AX_MONITOR_MODE_PMEPOL |
	       AX_MONITOR_MODE_RWMP;
	ax88179_write_cmd(dev, AX_ACCESS_MAC, AX_MONITOR_MOD, 1, 1, tmp);

	/* Configure default medium type => giga */
	*tmp16 = AX_MEDIUM_RECEIVE_EN | AX_MEDIUM_TXFLOW_CTRLEN |
		 AX_MEDIUM_RXFLOW_CTRLEN | AX_MEDIUM_FULL_DUPLEX |
		 AX_MEDIUM_GIGAMODE;
	ax88179_write_cmd(dev, AX_ACCESS_MAC, AX_MEDIUM_STATUS_MODE,
			  2, 2, tmp16);

	ax88179_led_setting(dev);

	ax179_data->eee_enabled = 0;
	ax179_data->eee_active = 0;

	ax88179_disable_eee(dev);

	ax88179_ethtool_get_eee(dev, &eee_data);
	eee_data.advertised = 0;
	ax88179_ethtool_set_eee(dev, &eee_data);

	/* Restart autoneg */
	mii_nway_restart(&dev->mii);

	usbnet_link_change(dev, 0, 0);

	return 0;
}