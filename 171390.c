static void ql_write_page2_reg(struct ql3_adapter *qdev,
			       u32 __iomem *reg, u32 value)
{
	if (qdev->current_page != 2)
		ql_set_register_page(qdev, 2);
	writel(value, reg);
	readl(reg);
}