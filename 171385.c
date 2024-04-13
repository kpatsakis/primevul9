static void ql_set_register_page(struct ql3_adapter *qdev, u32 page)
{
	struct ql3xxx_port_registers __iomem *port_regs =
		qdev->mem_map_registers;

	writel(((ISP_CONTROL_NP_MASK << 16) | page),
			&port_regs->CommonRegs.ispControlStatus);
	readl(&port_regs->CommonRegs.ispControlStatus);
	qdev->current_page = page;
}