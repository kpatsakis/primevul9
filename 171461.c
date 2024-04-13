static u8 ql_mii_disable_scan_mode(struct ql3_adapter *qdev)
{
	u8 ret;
	struct ql3xxx_port_registers __iomem *port_regs =
					qdev->mem_map_registers;

	/* See if scan mode is enabled before we turn it off */
	if (ql_read_page0_reg(qdev, &port_regs->macMIIMgmtControlReg) &
	    (MAC_MII_CONTROL_AS | MAC_MII_CONTROL_SC)) {
		/* Scan is enabled */
		ret = 1;
	} else {
		/* Scan is disabled */
		ret = 0;
	}

	/*
	 * When disabling scan mode you must first change the MII register
	 * address
	 */
	ql_write_page0_reg(qdev, &port_regs->macMIIMgmtAddrReg,
			   PHYAddr[0] | MII_SCAN_REGISTER);

	ql_write_page0_reg(qdev, &port_regs->macMIIMgmtControlReg,
			   ((MAC_MII_CONTROL_SC | MAC_MII_CONTROL_AS |
			     MAC_MII_CONTROL_RC) << 16));

	return ret;
}