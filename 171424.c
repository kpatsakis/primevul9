static u32 ql_get_link_state(struct ql3_adapter *qdev)
{
	struct ql3xxx_port_registers __iomem *port_regs =
			qdev->mem_map_registers;
	u32 bitToCheck = 0;
	u32 temp, linkState;

	switch (qdev->mac_index) {
	case 0:
		bitToCheck = PORT_STATUS_UP0;
		break;
	case 1:
		bitToCheck = PORT_STATUS_UP1;
		break;
	}

	temp = ql_read_page0_reg(qdev, &port_regs->portStatus);
	if (temp & bitToCheck)
		linkState = LS_UP;
	else
		linkState = LS_DOWN;

	return linkState;
}