static void ql_mii_enable_scan_mode(struct ql3_adapter *qdev)
{
	struct ql3xxx_port_registers __iomem *port_regs =
			qdev->mem_map_registers;
	u32 scanControl;

	if (qdev->numPorts > 1) {
		/* Auto scan will cycle through multiple ports */
		scanControl = MAC_MII_CONTROL_AS | MAC_MII_CONTROL_SC;
	} else {
		scanControl = MAC_MII_CONTROL_SC;
	}

	/*
	 * Scan register 1 of PHY/PETBI,
	 * Set up to scan both devices
	 * The autoscan starts from the first register, completes
	 * the last one before rolling over to the first
	 */
	ql_write_page0_reg(qdev, &port_regs->macMIIMgmtAddrReg,
			   PHYAddr[0] | MII_SCAN_REGISTER);

	ql_write_page0_reg(qdev, &port_regs->macMIIMgmtControlReg,
			   (scanControl) |
			   ((MAC_MII_CONTROL_SC | MAC_MII_CONTROL_AS) << 16));
}