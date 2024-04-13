static void ql_enable_interrupts(struct ql3_adapter *qdev)
{
	struct ql3xxx_port_registers __iomem *port_regs =
		qdev->mem_map_registers;

	ql_write_common_reg_l(qdev, &port_regs->CommonRegs.ispInterruptMaskReg,
			    ((0xff << 16) | ISP_IMR_ENABLE_INT));

}