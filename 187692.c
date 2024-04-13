static int yam_set_mac_address(struct net_device *dev, void *addr)
{
	struct sockaddr *sa = (struct sockaddr *) addr;

	/* addr is an AX.25 shifted ASCII mac address */
	dev_addr_set(dev, sa->sa_data);
	return 0;
}