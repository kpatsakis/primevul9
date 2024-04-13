
static int ql3xxx_set_mac_address(struct net_device *ndev, void *p)
{
	struct ql3_adapter *qdev = netdev_priv(ndev);
	struct ql3xxx_port_registers __iomem *port_regs =
			qdev->mem_map_registers;
	struct sockaddr *addr = p;
	unsigned long hw_flags;

	if (netif_running(ndev))
		return -EBUSY;

	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;

	memcpy(ndev->dev_addr, addr->sa_data, ndev->addr_len);

	spin_lock_irqsave(&qdev->hw_lock, hw_flags);
	/* Program lower 32 bits of the MAC address */
	ql_write_page0_reg(qdev, &port_regs->macAddrIndirectPtrReg,
			   (MAC_ADDR_INDIRECT_PTR_REG_RP_MASK << 16));
	ql_write_page0_reg(qdev, &port_regs->macAddrDataReg,
			   ((ndev->dev_addr[2] << 24) | (ndev->
							 dev_addr[3] << 16) |
			    (ndev->dev_addr[4] << 8) | ndev->dev_addr[5]));

	/* Program top 16 bits of the MAC address */
	ql_write_page0_reg(qdev, &port_regs->macAddrIndirectPtrReg,
			   ((MAC_ADDR_INDIRECT_PTR_REG_RP_MASK << 16) | 1));
	ql_write_page0_reg(qdev, &port_regs->macAddrDataReg,
			   ((ndev->dev_addr[0] << 8) | ndev->dev_addr[1]));
	spin_unlock_irqrestore(&qdev->hw_lock, hw_flags);

	return 0;