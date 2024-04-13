static int ql_mii_read_reg(struct ql3_adapter *qdev, u16 regAddr, u16 *value)
{
	u32 temp;
	struct ql3xxx_port_registers __iomem *port_regs =
			qdev->mem_map_registers;

	ql_mii_disable_scan_mode(qdev);

	if (ql_wait_for_mii_ready(qdev)) {
		netif_warn(qdev, link, qdev->ndev, TIMED_OUT_MSG);
		return -1;
	}

	ql_write_page0_reg(qdev, &port_regs->macMIIMgmtAddrReg,
			   qdev->PHYAddr | regAddr);

	ql_write_page0_reg(qdev, &port_regs->macMIIMgmtControlReg,
			   (MAC_MII_CONTROL_RC << 16));

	ql_write_page0_reg(qdev, &port_regs->macMIIMgmtControlReg,
			   (MAC_MII_CONTROL_RC << 16) | MAC_MII_CONTROL_RC);

	/* Wait for the read to complete */
	if (ql_wait_for_mii_ready(qdev)) {
		netif_warn(qdev, link, qdev->ndev, TIMED_OUT_MSG);
		return -1;
	}

	temp = ql_read_page0_reg(qdev, &port_regs->macMIIMgmtDataReg);
	*value = (u16) temp;

	ql_mii_enable_scan_mode(qdev);

	return 0;
}