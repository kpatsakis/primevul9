static u32 ql_read_page0_reg_l(struct ql3_adapter *qdev, u32 __iomem *reg)
{
	u32 value;
	unsigned long hw_flags;

	spin_lock_irqsave(&qdev->hw_lock, hw_flags);

	if (qdev->current_page != 0)
		ql_set_register_page(qdev, 0);
	value = readl(reg);

	spin_unlock_irqrestore(&qdev->hw_lock, hw_flags);
	return value;
}