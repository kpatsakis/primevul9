static int ql_mii_write_reg_ex(struct ql3_adapter *qdev,
			       u16 regAddr, u16 value, u32 phyAddr)
{
	struct ql3xxx_port_registers __iomem *port_regs =
			qdev->mem_map_registers;
	u8 scanWasEnabled;

	scanWasEnabled = ql_mii_disable_scan_mode(qdev);

	if (ql_wait_for_mii_ready(qdev)) {
		netif_warn(qdev, link, qdev->ndev, TIMED_OUT_MSG);
		return -1;
	}

	ql_write_page0_reg(qdev, &port_regs->macMIIMgmtAddrReg,
			   phyAddr | regAddr);

	ql_write_page0_reg(qdev, &port_regs->macMIIMgmtDataReg, value);

	/* Wait for write to complete 9/10/04 SJP */
	if (ql_wait_for_mii_ready(qdev)) {
		netif_warn(qdev, link, qdev->ndev, TIMED_OUT_MSG);
		return -1;
	}

	if (scanWasEnabled)
		ql_mii_enable_scan_mode(qdev);

	return 0;
}