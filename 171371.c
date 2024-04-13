static int ql_mii_setup(struct ql3_adapter *qdev)
{
	u32 reg;
	struct ql3xxx_port_registers __iomem *port_regs =
			qdev->mem_map_registers;

	if (ql_sem_spinlock(qdev, QL_PHY_GIO_SEM_MASK,
			(QL_RESOURCE_BITS_BASE_CODE | (qdev->mac_index) *
			 2) << 7))
		return -1;

	if (qdev->device_id == QL3032_DEVICE_ID)
		ql_write_page0_reg(qdev,
			&port_regs->macMIIMgmtControlReg, 0x0f00000);

	/* Divide 125MHz clock by 28 to meet PHY timing requirements */
	reg = MAC_MII_CONTROL_CLK_SEL_DIV28;

	ql_write_page0_reg(qdev, &port_regs->macMIIMgmtControlReg,
			   reg | ((MAC_MII_CONTROL_CLK_SEL_MASK) << 16));

	ql_sem_unlock(qdev, QL_PHY_GIO_SEM_MASK);
	return 0;
}