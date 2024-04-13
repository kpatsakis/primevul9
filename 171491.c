static int ql_link_down_detect_clear(struct ql3_adapter *qdev)
{
	struct ql3xxx_port_registers __iomem *port_regs =
			qdev->mem_map_registers;

	switch (qdev->mac_index) {
	case 0:
		ql_write_common_reg(qdev,
				    &port_regs->CommonRegs.ispControlStatus,
				    (ISP_CONTROL_LINK_DN_0) |
				    (ISP_CONTROL_LINK_DN_0 << 16));
		break;

	case 1:
		ql_write_common_reg(qdev,
				    &port_regs->CommonRegs.ispControlStatus,
				    (ISP_CONTROL_LINK_DN_1) |
				    (ISP_CONTROL_LINK_DN_1 << 16));
		break;

	default:
		return 1;
	}

	return 0;
}