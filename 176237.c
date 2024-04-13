static void ax88179_get_mac_addr(struct usbnet *dev)
{
	u8 mac[ETH_ALEN];

	memset(mac, 0, sizeof(mac));

	/* Maybe the boot loader passed the MAC address via device tree */
	if (!eth_platform_get_mac_address(&dev->udev->dev, mac)) {
		netif_dbg(dev, ifup, dev->net,
			  "MAC address read from device tree");
	} else {
		ax88179_read_cmd(dev, AX_ACCESS_MAC, AX_NODE_ID, ETH_ALEN,
				 ETH_ALEN, mac);
		netif_dbg(dev, ifup, dev->net,
			  "MAC address read from ASIX chip");
	}

	if (is_valid_ether_addr(mac)) {
		eth_hw_addr_set(dev->net, mac);
	} else {
		netdev_info(dev->net, "invalid MAC address, using random\n");
		eth_hw_addr_random(dev->net);
	}

	ax88179_write_cmd(dev, AX_ACCESS_MAC, AX_NODE_ID, ETH_ALEN, ETH_ALEN,
			  dev->net->dev_addr);
}