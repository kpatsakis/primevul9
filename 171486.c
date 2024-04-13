static u32 ql_read_common_reg_l(struct ql3_adapter *qdev, u32 __iomem *reg)
{
	u32 value;
	unsigned long hw_flags;

	spin_lock_irqsave(&qdev->hw_lock, hw_flags);
	value = readl(reg);
	spin_unlock_irqrestore(&qdev->hw_lock, hw_flags);

	return value;
}