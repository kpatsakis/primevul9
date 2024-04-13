static int ql_auto_neg_error(struct ql3_adapter *qdev)
{
	struct ql3xxx_port_registers __iomem *port_regs =
			qdev->mem_map_registers;
	u32 bitToCheck = 0;
	u32 temp;

	switch (qdev->mac_index) {
	case 0:
		bitToCheck = PORT_STATUS_AE0;
		break;
	case 1:
		bitToCheck = PORT_STATUS_AE1;
		break;
	}
	temp = ql_read_page0_reg(qdev, &port_regs->portStatus);
	return (temp & bitToCheck) != 0;
}