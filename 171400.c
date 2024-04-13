static void ql_write_page0_reg(struct ql3_adapter *qdev,
			       u32 __iomem *reg, u32 value)
{
	if (qdev->current_page != 0)
		ql_set_register_page(qdev, 0);
	writel(value, reg);
	readl(reg);
}