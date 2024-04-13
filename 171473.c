static u32 ql_read_page0_reg(struct ql3_adapter *qdev, u32 __iomem *reg)
{
	if (qdev->current_page != 0)
		ql_set_register_page(qdev, 0);
	return readl(reg);
}