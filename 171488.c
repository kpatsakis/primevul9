static int ql_wait_for_mii_ready(struct ql3_adapter *qdev)
{
	struct ql3xxx_port_registers __iomem *port_regs =
			qdev->mem_map_registers;
	u32 temp;
	int count = 1000;

	while (count) {
		temp = ql_read_page0_reg(qdev, &port_regs->macMIIStatusReg);
		if (!(temp & MAC_MII_STATUS_BSY))
			return 0;
		udelay(10);
		count--;
	}
	return -1;
}