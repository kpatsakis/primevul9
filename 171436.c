static int ql_link_down_detect(struct ql3_adapter *qdev)
{
	struct ql3xxx_port_registers __iomem *port_regs =
			qdev->mem_map_registers;
	u32 bitToCheck = 0;
	u32 temp;

	switch (qdev->mac_index) {
	case 0:
		bitToCheck = ISP_CONTROL_LINK_DN_0;
		break;
	case 1:
		bitToCheck = ISP_CONTROL_LINK_DN_1;
		break;
	}

	temp =
	    ql_read_common_reg(qdev, &port_regs->CommonRegs.ispControlStatus);
	return (temp & bitToCheck) != 0;
}