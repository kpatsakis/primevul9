static void ql_set_mac_addr(struct net_device *ndev, u16 *addr)
{
	__le16 *p = (__le16 *)ndev->dev_addr;
	p[0] = cpu_to_le16(addr[0]);
	p[1] = cpu_to_le16(addr[1]);
	p[2] = cpu_to_le16(addr[2]);
}