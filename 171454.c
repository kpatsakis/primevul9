static void ql_write_page1_reg(struct ql3_adapter *qdev,
			       u32 __iomem *reg, u32 value)
{
	if (qdev->current_page != 1)
		ql_set_register_page(qdev, 1);
	writel(value, reg);
	readl(reg);
}