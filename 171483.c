
static void ql_get_pauseparam(struct net_device *ndev,
			      struct ethtool_pauseparam *pause)
{
	struct ql3_adapter *qdev = netdev_priv(ndev);
	struct ql3xxx_port_registers __iomem *port_regs =
		qdev->mem_map_registers;

	u32 reg;
	if (qdev->mac_index == 0)
		reg = ql_read_page0_reg(qdev, &port_regs->mac0ConfigReg);
	else
		reg = ql_read_page0_reg(qdev, &port_regs->mac1ConfigReg);

	pause->autoneg  = ql_get_auto_cfg_status(qdev);
	pause->rx_pause = (reg & MAC_CONFIG_REG_RF) >> 2;
	pause->tx_pause = (reg & MAC_CONFIG_REG_TF) >> 1;